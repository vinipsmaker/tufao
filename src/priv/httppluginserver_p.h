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
#include <QtCore/QStringList>

namespace Tufao {

class ConfigFile
{
public:
    const QFileSystemWatcher &watcher() const;

    bool setFile(const QString &file);

    QString file() const;

    void clear();

private:
    QString file_;
    QFileSystemWatcher watcher_;
};

class ConfigContent
{
public:
    struct Plugin
    {
        Plugin() = default;

        template<class T>
        Plugin(T &&name, T &&path) :
            name(std::forward<T>(name)), path(std::forward<T>(path))
        {}

        bool operator ==(const Plugin &o) const;

        bool operator !=(const Plugin &o) const;

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

    bool load(const QString &fileName);

    // The list is sorted by loading order
    const QList<Plugin> &plugins() const;

    const QList<Request> &requests() const;

    void clear();

private:
    // Check if the config is valid.
    // Don't check if the config works in this system (files exists...).
    static bool isConfigValid(const QJsonDocument &doc,
                              DependencyTree<QString> &dependencies);

    QList<Plugin> plugins_;
    QList<Request> requests_;
};

struct HttpPluginServer::Priv
{
    Priv(HttpPluginServer *parent);
    Priv();
    ~Priv();

    struct Plugin
    {
        HttpServerPlugin *plugin;
        std::function<bool(HttpServerRequest&, HttpServerResponse&)> handler;
    };

    ConfigFile configFile;
    ConfigContent configContent;

    QList<QPluginLoader*> plugins;
    QHash<QString, Plugin> handlers;
    Tufao::HttpServerRequestRouter *router;


    static void loadNewConfig (Tufao::HttpServerRequestRouter *router,const ConfigContent &configContent, QList<QPluginLoader *> &plugins, QHash<QString, Plugin> &handlers);
};

} // namespace Tufao

#endif // TUFAO_PRIV_HTTPPLUGINSERVER_H
