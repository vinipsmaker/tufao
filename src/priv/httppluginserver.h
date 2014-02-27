/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_PRIV_HTTPPLUGINSERVER_H
#define TUFAO_PRIV_HTTPPLUGINSERVER_H

#include "../httppluginserver.h"
#include "../httpserverrequestrouter.h"
#include "../httpserverplugin.h"

#include "dependencytree.h"

#include <QtCore/QFileSystemWatcher>
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>

namespace Tufao {

class ConfigFile
{
public:
    const QFileSystemWatcher &watcher() const
    {
        return watcher_;
    }

    bool setFile(const QString &file)
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

    bool setDir(const QString &file/*not dir*/)
    {
        if (!setFile(QFileInfo(file).path()))
            return false;

        file_ = file;
        return true;
    }

    QString file() const
    {
        return file_;
    }

    void clear()
    {
        watcher_.removePath(file_);
        file_.clear();
    }

private:
    QString file_;
    QFileSystemWatcher watcher_;
};

class ConfigContent
{
public:
    enum class MonitoringBehaviour
    {
        BASIC,
        FULL
    };

    struct Plugin
    {
        Plugin() = default;

        template<class T>
        Plugin(T &&name, T &&path) :
            name(std::forward<T>(name)), path(std::forward<T>(path))
        {}

        bool operator ==(const Plugin &o) const
        {
            return name == o.name && path == o.path
                    && dependencies == o.dependencies
                    && customData == o.customData;
        }

        bool operator !=(const Plugin &o) const
        {
            return !(*this == o);
        }

        QString name;
        QString path;
        QStringList dependencies;
        QVariant customData;
    };

    struct Request
    {
        Request() = default;

        template<class T>
        Request(T &&path, T &&plugin) :
            path(std::forward<T>(path)), plugin(std::forward<T>(plugin))
        {}

        QString path;
        QString plugin;
        QString method;
    };

    bool load(const QString &fileName)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
            return false;

        QJsonParseError parseError;
        QJsonDocument doc(QJsonDocument::fromJson(file.readAll(), &parseError));
        if (parseError.error != QJsonParseError::NoError)
            return false;

        DependencyTree<QString> dependencies;
        if (!isConfigValid(doc, dependencies))
            return false;

        plugins_.clear();
        requests_.clear();

        monitoringBehaviour_ = doc.object()["version"].toDouble() == 1.
            ? MonitoringBehaviour::FULL : MonitoringBehaviour::BASIC;

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

    /*!
     * This property is special and isn't cleared through clear().
     */
    MonitoringBehaviour monitoringBehaviour() const
    {
        return monitoringBehaviour_;
    }

    // The list is sorted by loading order
    const QList<Plugin> &plugins() const
    {
        return plugins_;
    }

    const QList<Request> &requests() const
    {
        return requests_;
    }

    void clear()
    {
        plugins_.clear();
        requests_.clear();
    }

private:
    // Check if the config is valid.
    // Don't check if the config works in this system (files exists...).
    static bool isConfigValid(const QJsonDocument &doc,
                              DependencyTree<QString> &dependencies)
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
            if (!version.isDouble()
                || (version.toDouble() != 0. && version.toDouble() != 1.)) {
                return false;
            }
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

    MonitoringBehaviour monitoringBehaviour_;
    QList<Plugin> plugins_;
    QList<Request> requests_;
};

struct HttpPluginServer::Priv
{
    struct Plugin
    {
        HttpServerPlugin *plugin;
        std::function<bool(HttpServerRequest&, HttpServerResponse&)> handler;
    };

    ConfigFile configFile;
    ConfigContent configContent;

    QList<QPluginLoader*> plugins;
    QHash<QString, Plugin> handlers;
    Tufao::HttpServerRequestRouter router;
};

} // namespace Tufao

#endif // TUFAO_PRIV_HTTPPLUGINSERVER_H
