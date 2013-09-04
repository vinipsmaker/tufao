#ifndef THREADEDHTTPPLUGINSERVER_P_H
#define THREADEDHTTPPLUGINSERVER_P_H

#include "../threadedhttppluginserver.h"
#include "httppluginserver_p.h"
#include "threadedhttprequestdispatcher.h"

namespace Tufao{

struct ThreadedHttpPluginServer::Priv : public ThreadedHttpRequestDispatcher::Priv {
    Priv(ThreadedHttpPluginServer* parent): ThreadedHttpRequestDispatcher::Priv(parent){}
    virtual ~Priv() = default;

    ThreadedHttpPluginServer::Factory     factoryFunc;

    ConfigFile configFile;
    ConfigContent configContent;
};

struct ThreadData{
        ThreadData() : userCustomData(0) {}

        QList<QPluginLoader*> plugins;
        QHash<QString, HttpPluginServer::Priv::Plugin> handlers;
        void *userCustomData;
};

}

#endif // THREADEDHTTPPLUGINSERVER_P_H
