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

#include "priv/httppluginserver.h"

#include "httpserverresponse.h"

#include <utility>

namespace Tufao {

HttpPluginServer::HttpPluginServer(QObject *parent):
    QObject(parent),
    priv(new Priv)
{
    connect(&priv->configFile.watcher(), &QFileSystemWatcher::fileChanged,
            this, &HttpPluginServer::onConfigFileChanged);
    connect(&priv->configFile.watcher(),&QFileSystemWatcher::directoryChanged,
            this, &HttpPluginServer::onConfigDirChanged);
}

HttpPluginServer::HttpPluginServer(const QString &configFile, QObject *parent):
    HttpPluginServer(parent)
{
    if (priv->configFile.setFile(configFile))
        reloadConfig();
}

HttpPluginServer::~HttpPluginServer()
{
    clear();
    delete priv;
}

bool HttpPluginServer::setConfig(const QString &file)
{
    clear();

    if (file.isEmpty())
        return true;

    if (!priv->configFile.setFile(file))
        return false;

    reloadConfig();
    return true;
}

QString HttpPluginServer::config() const
{
    return priv->configFile.file();
}

bool HttpPluginServer::handleRequest(HttpServerRequest &request,
                                     HttpServerResponse &response)
{
    return priv->router.handleRequest(request, response);
}

void HttpPluginServer::onConfigFileChanged()
{
    if (!QFileInfo(priv->configFile.file()).exists()) {
        if (priv->configContent.monitoringBehaviour()
            == ConfigContent::MonitoringBehaviour::BASIC ) {
            clear();
        } else {
            QString file = priv->configFile.file();
            clear();
            priv->configFile.setDir(file);
        }
        return;
    }

    reloadConfig();
}

void HttpPluginServer::onConfigDirChanged()
{
    if (!QFileInfo(priv->configFile.file()).exists())
        return;

    priv->configFile.setFile(priv->configFile.file());
    reloadConfig();
}

inline void HttpPluginServer::clear()
{
    priv->configFile.clear();
    priv->configContent.clear();
    priv->router.clear();
    priv->handlers.clear();

    for (const auto &p: priv->plugins) {
        p->unload();
        p->deleteLater();
    }

    priv->plugins.clear();
}

inline void HttpPluginServer::reloadConfig()
{
    {
        ConfigContent content;
        if (!content.load(priv->configFile.file())) {
            qWarning("Tufao::HttpPluginServer: Couldn't load new config from"
                     " \"%s\"", qPrintable(priv->configFile.file()));
            return;
        }

        std::swap(content, priv->configContent);
    }

    // Reset the request handlers
    priv->router.clear();
    priv->handlers.clear();

    for (const auto &p: priv->plugins) {
        p->unload();
        p->deleteLater();
    }

    priv->plugins.clear();

    // Load the new config
    for (const auto &p: priv->configContent.plugins()) {
        QPluginLoader *loader = new QPluginLoader(p.path, this);
        auto warn = [&p]() {
            qWarning("Tufao::HttpPluginServer: Couldn't load plugin"
                     " \"%s\"", qPrintable(p.path));
        };
        auto warnAndClear = [&warn,loader]() {
            warn();
            loader->unload();
            loader->deleteLater();
        };

        if (!loader->load()) {
            warn();
            loader->deleteLater();
            continue;
        }

        auto plugin = qobject_cast<HttpServerPlugin*>(loader->instance());

        if (!plugin) {
            warnAndClear();
            continue;
        }

        HttpServerRequestRouter::Handler handler;
        {
            QHash<QString, HttpServerPlugin*> dependencies;
            bool ok = true;

            for (const auto &d: p.dependencies) {
                if (!priv->handlers.contains(d)) {
                    ok = false;
                    break;
                }

                dependencies[d] = priv->handlers[d].plugin;
            }

            if (!ok) {
                warnAndClear();
                continue;
            }

            handler = plugin->createHandler(dependencies, p.customData);
        }

        priv->handlers[p.name] = {plugin, std::move(handler)};
        priv->plugins += loader;
    }

    for (const auto &r: priv->configContent.requests()) {
        if (!priv->handlers.contains(r.plugin)) {
            qWarning("Tufao::HttpPluginServer: Plugin not loaded: \"%s\"",
                     qPrintable(r.plugin));
            continue;
        }

        auto path = QRegularExpression{r.path};
        const auto &handler = priv->handlers[r.plugin].handler;

        if (r.method.size())
            priv->router.map({path, r.method.toUtf8(), handler});
        else
            priv->router.map({path, handler});
    }
}

} // namespace Tufao
