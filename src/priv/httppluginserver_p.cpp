#include "httppluginserver_p.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>

namespace Tufao{

const QFileSystemWatcher &ConfigFile::watcher() const
{
    return watcher_;
}

bool ConfigFile::setFile(const QString &file)
{
    // integrity checks
    QFileInfo info(file);
    if (!info.exists())
        return false;

    // clean old resources
    if (file_.size())
        watcher_.removePath(file_);

    // use new resources
    watcher_.addPath(file);
    file_ = file;

    return true;
}

QString ConfigFile::file() const
{
    return file_;
}

void ConfigFile::clear()
{
    watcher_.removePath(file_);
    file_.clear();
}

bool ConfigContent::Plugin::operator ==(const ConfigContent::Plugin &o) const
{
    return name == o.name && path == o.path
            && dependencies == o.dependencies
            && customData == o.customData;
}

bool ConfigContent::Plugin::operator !=(const ConfigContent::Plugin &o) const
{
    return !(*this == o);
}

bool ConfigContent::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(file.readAll(), &parseError));
    if (parseError.error != QJsonParseError::NoError){
        QString fName = fileName;
        qWarning()<<"Tufao::ThreadedHttpPluginServer: Parse Error \n"
                  <<"File: \""  <<fileName<<"\"\n"
                  <<"Error: \"" <<parseError.errorString()<<"\"\n"
                  <<"Offset: \""<<parseError.offset<<"\"";
        return false;
    }

    DependencyTree<QString> dependencies;
    if (!isConfigValid(doc, dependencies))
        return false;

    plugins_.clear();
    requests_.clear();

    QHash<QString, Plugin> plugins;

    for (const auto &element: doc.object()["plugins"].toArray()) {
        auto plugin = element.toObject();
        Plugin p{plugin["name"].toString(), plugin["path"].toString()};

        if (plugin.contains("dependencies")) {
            QStringList dependencies;

            {
                auto array = plugin["dependencies"].toArray();
                dependencies.reserve(array.size());
                for (const auto &d: array) {
                    dependencies += d.toString();
                }
            }

            p.dependencies = dependencies;
        }

        if (plugin.contains("customData"))
            p.customData = plugin.value("customData").toVariant();

        plugins[p.name] = p;
    }

    for (const auto &element: doc.object()["requests"].toArray()) {
        auto request = element.toObject();
        Request r{request["path"].toString(), request["plugin"].toString()};

        if (request.contains("method"))
            r.method = request["method"].toString();

        requests_ += r;
    }

    for (const auto &p: dependencies.sorted())
        plugins_ += plugins[p];

    return true;
}

const QList<ConfigContent::Plugin> &ConfigContent::plugins() const
{
    return plugins_;
}

const QList<ConfigContent::Request> &ConfigContent::requests() const
{
    return requests_;
}

void ConfigContent::clear()
{
    plugins_.clear();
    requests_.clear();
}

bool ConfigContent::isConfigValid(const QJsonDocument &doc, DependencyTree<QString> &dependencies)
{
    // > check root structure
    if (!doc.isObject())
        return false;

    QJsonObject root(doc.object());
    if (!root.contains("version") || !root.contains("plugins")
            || !root.contains("requests")) {
        return false;
    }

    // >> check version
    {
        auto version = root["version"];
        if (!version.isDouble() || version.toDouble() != 0.)
            return false;
    }

    // >> check plugins
    {
        auto plugins = root["plugins"];
        if (!plugins.isArray())
            return false;

        QHash<QString, QString> pluginMetadata;

        for (auto &&plugin: plugins.toArray()) {
            if (!plugin.isObject())
                return false;

            auto obj = plugin.toObject();
            if (!obj.contains("name") || !obj.contains("path"))
                return false;

            // >>> check name
            {
                auto name = obj["name"];
                if (!name.isString())
                    return false;

                auto string = name.toString();
                if (string.isEmpty() || pluginMetadata.contains(string))
                    return false;
            }

            // >>> check path
            {
                auto path = obj["path"];
                if (!path.isString())
                    return false;

                auto string = path.toString();
                if (string.isEmpty())
                    return false;

                pluginMetadata[obj["name"].toString()] = string;
            }

            // >>> check dependencies
            if (obj.contains("dependencies")) {
                auto value = obj["dependencies"];
                if (!value.isArray())
                    return false;

                QStringList currentDependencies;
                for (const auto &dependency: value.toArray()) {
                    if (!dependency.isString())
                        return false;

                    auto current = dependency.toString();
                    if (current.isEmpty())
                        return false;

                    currentDependencies.push_back(current);
                }

                if (!dependencies.addNode(obj["name"].toString(),
                                          currentDependencies)) {
                    return false;
                }
            } else {
                if (!dependencies.addNode(obj["name"].toString()))
                    return false;
            }
        }

        // check if dependencies are present
        if (dependencies.hasUnsatisfiedDependency())
            return false;
    }

    // >> check requests
    {
        auto requests = root["requests"];
        if (!requests.isArray())
            return false;

        for (auto &&request: requests.toArray()) {
            if (!request.isObject())
                return false;

            auto obj = request.toObject();
            if (!obj.contains("path") || !obj.contains("plugin"))
                return false;

            // >>> check path
            {
                auto path = obj["path"];
                if (!path.isString())
                    return false;

                auto string = path.toString();
                if (!QRegularExpression{string}.isValid())
                    return false;
            }

            // >>> check plugin
            {
                auto plugin = obj["plugin"];
                if (!plugin.isString() || plugin.toString().isEmpty())
                    return false;
            }

            // >>> check method
            if (obj.contains("method")) {
                auto method = obj["method"];
                if (!method.isString())
                    return false;
            }
        }
    }

    return true;
}

/*!
  \brief HttpPluginServer::Priv::Priv
  \param parent
  Called by HttpPluginServer constructor
 */
HttpPluginServer::Priv::Priv(HttpPluginServer *parent)
    : router(new HttpServerRequestRouter(parent))
{

}

/*!
  \brief HttpPluginServer::Priv::Priv
  Called by ThreadedHttpPluginServer constructor
 */
HttpPluginServer::Priv::Priv()
    : router(0)
{

}

HttpPluginServer::Priv::~Priv()
{
    //make sure we delete router only when it has no parent
    if(router && !router->parent())
        delete router;
}


/**
   \brief HttpPluginServer::Priv::loadNewConfig
   \param router
   \param configContent
   \param plugins
   \param handlers
   \warning never change any global stuff here this needs to be reentrant
   Moved from public implementation to share with the threaded PluginServer
 */
void HttpPluginServer::Priv::loadNewConfig(HttpServerRequestRouter *router, const ConfigContent &configContent, QList<QPluginLoader*> &plugins, QHash<QString, Plugin> &handlers)
{
    for (const auto &p: configContent.plugins()) {
        QPluginLoader *loader = new QPluginLoader(p.path);
        auto warn = [&p,loader]() {
            qWarning("Tufao::HttpPluginServer: Couldn't load plugin"
                     " \"%s\"", qPrintable(p.path));
            loader->deleteLater();
        };

        if (!loader->load()) {
            warn();
            continue;
        }

        auto plugin = qobject_cast<HttpServerPlugin*>(loader->instance());

        if (!plugin) {
            warn();
            continue;
        }

        HttpServerRequestRouter::Handler handler;
        {
            QHash<QString, HttpServerPlugin*> dependencies;
            bool ok = true;

            for (const auto &d: p.dependencies) {
                if (!handlers.contains(d)) {
                    ok = false;
                    break;
                }

                dependencies[d] = handlers[d].plugin;
            }

            if (!ok) {
                warn();
                continue;
            }

            handler = plugin->createHandler(dependencies, p.customData);
        }

        handlers[p.name] = {plugin, std::move(handler)};
        plugins += loader;
    }

    for (const auto &r: configContent.requests()) {
        if (!handlers.contains(r.plugin)) {
            qWarning("Tufao::HttpPluginServer: Plugin not loaded: \"%s\"",
                     qPrintable(r.plugin));
            continue;
        }

        auto path = QRegularExpression{r.path};
        const auto &handler = handlers[r.plugin].handler;

        if (r.method.size())
            router->map({path, r.method.toUtf8(), handler});
        else
            router->map({path, handler});
    }
}

}
