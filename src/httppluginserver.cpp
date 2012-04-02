/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "httppluginserver.h"
#include "priv/httppluginserver.h"
#include "abstracthttpserverrequesthandlerloader.h"
#include <QtCore/QSettings>

namespace Tufao {

HttpPluginServer::HttpPluginServer(const QString &configFile, QObject *parent):
    AbstractHttpServerRequestHandler(parent),
    priv(new Priv::HttpPluginServer)
{
    if (!configFile.isEmpty()) {
        priv->configFile = configFile;
        reloadConfig();
    }
}

HttpPluginServer::~HttpPluginServer()
{
    clear();
    delete priv;
}

void HttpPluginServer::setConfig(const QString &file)
{
    priv->configFile = file;
    reloadConfig();
}

QString HttpPluginServer::config() const
{
    return priv->configFile;
}

bool HttpPluginServer::handleRequest(HttpServerRequest *request,
                                     HttpServerResponse *response,
                                     const QStringList &args)
{
    return priv->router.handleRequest(request, response, args);
}

void HttpPluginServer::reloadConfig()
{
    clear();

    QSettings settings(priv->configFile, QSettings::IniFormat);
    if (settings.value("version").toString() != "0.1") {
        qWarning("Unsupported config file version");
        return;
    }

    int size = settings.beginReadArray("plugins");
    for (int i = 0;i != size;++i) {
        settings.setArrayIndex(i);
        QPluginLoader *plugin = new QPluginLoader(settings.value("filename")
                                                  .toString());
        if (!plugin->load()) {
            qWarning("Couldn't load plugin");
            delete plugin;
            continue;
        }

        AbstractHttpServerRequestHandlerLoader *loader =
                qobject_cast<AbstractHttpServerRequestHandlerLoader *>
                (plugin->instance());

        if (!loader) {
            qWarning("Plugin doesn't implement"
                     " AbstractHttpServerRequestHandlerLoader interface");
            plugin->unload();
            delete plugin;
            continue;
        }

        AbstractHttpServerRequestHandler *handler = loader->createHandler(this);
        if (!handler) {
            qWarning("Plugin returned a null handler");
            plugin->unload();
            delete plugin;
            continue;
        }

        QByteArray method = settings.value("method").toByteArray();
        if (method.isEmpty()) {
            priv->router.map(settings.value("regex").toRegExp(), handler);
        } else {
            priv->router.map(settings.value("regex").toRegExp(), method,
                             handler);
        }

        priv->plugins.push_back(plugin);
        priv->handlers.push_back(handler);
    }
}

inline void HttpPluginServer::clear()
{
    priv->router.clear();

    for (int i = 0;i != priv->handlers.size();++i)
        priv->handlers[i]->deleteLater();
    priv->handlers.clear();

    for (int i = 0;i != priv->plugins.size();++i) {
        priv->plugins[i]->unload();
        delete priv->plugins[i];
    }
    priv->plugins.clear();
}

} // namespace Tufao
