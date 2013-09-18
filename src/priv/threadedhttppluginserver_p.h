#ifndef THREADEDHTTPPLUGINSERVER_P_H
#define THREADEDHTTPPLUGINSERVER_P_H

#include "../threadedhttppluginserver.h"
#include "httppluginserver_p.h"
#include "threadedhttpserver_p.h"

namespace Tufao{

struct ThreadedHttpPluginServer::Priv : public ThreadedHttpServer::Priv {
    Priv() = default;
    virtual ~Priv() = default;


    ConfigFile configFile;
    ConfigContent configContent;

    // ThreadedHttpServer::Priv interface
    public:
    virtual void startThreadPool();
};

struct ThreadData{
        ThreadData() : userCustomData(0) {}

        QList<QPluginLoader*> plugins;
        QHash<QString, HttpPluginServer::Priv::Plugin> handlers;
        void *userCustomData;
};

}

#endif // THREADEDHTTPPLUGINSERVER_P_H
