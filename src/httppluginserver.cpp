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

#include "priv/httppluginserver_p.h"

#include "httpserverresponse.h"

#include <utility>

namespace Tufao {

HttpPluginServer::HttpPluginServer(QObject *parent):
    QObject(parent),
    priv(new Priv(this))
{
    connect(&priv->configFile.watcher(), &QFileSystemWatcher::fileChanged,
            this, &HttpPluginServer::onConfigFileChanged);
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
    return priv->router->handleRequest(request, response);
}

void HttpPluginServer::onConfigFileChanged()
{
    if (!QFileInfo(priv->configFile.file()).exists()) {
        clear();
        return;
    }

    reloadConfig();
}

inline void HttpPluginServer::clear()
{
    priv->configFile.clear();
    priv->configContent.clear();
    priv->router->clear();

    for (const auto &p: priv->plugins)
        p->deleteLater();

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
    priv->router->clear();

    for (const auto &p: priv->plugins){
        p->unload();
        p->deleteLater();
    }

    priv->plugins.clear();
    priv->handlers.clear();

    // Load the new config
    HttpPluginServer::Priv::loadNewConfig(priv->router,priv->configContent,priv->plugins,priv->handlers);
}

} // namespace Tufao
