#include "priv/threadedhttppluginserver_p.h"
#include "httpserverrequestrouter.h"
#include <QPluginLoader>
#include "abstractconnectionhandler.h"

namespace Tufao {

ThreadedHttpPluginServer::ThreadedHttpPluginServer(QObject *parent)
    : ThreadedHttpServer(new Priv,parent)
{
    ThreadedHttpPluginServer::Priv* p = ((Priv*)_priv());

    connect(&p->configFile.watcher(), &QFileSystemWatcher::fileChanged,
            this, &ThreadedHttpPluginServer::onConfigFileChanged);

}

bool ThreadedHttpPluginServer::setConfig(const QString &file)
{
    //TODO use a internal clear , let public clear do a real shutdown
    clear();

    if (file.isEmpty())
        return true;

    if (!((Priv*)_priv())->configFile.setFile(file))
        return false;

    loadConfig();
    return true;
}

QString ThreadedHttpPluginServer::config() const
{
    return ((Priv*)_priv())->configFile.file();
}

void ThreadedHttpPluginServer::loadConfig()
{
    ThreadedHttpPluginServer::Priv* p = ((Priv*)_priv());

    //reload config
    p->startThreadPool();

    p->threadPool->pauseDispatch(false); //start request dispatching
}

void ThreadedHttpPluginServer::onConfigFileChanged()
{

    clear();

    if (!QFileInfo(((Priv*)_priv())->configFile.file()).exists()) {
        return;
    }

    loadConfig();
}

void ThreadedHttpPluginServer::clear()
{
    ThreadedHttpPluginServer::Priv* p = ((Priv*)_priv());

    p->threadPool->pauseDispatch(true);
    p->threadPool->stopAllThreads(); //shut down all threads

    p->configFile.clear();
    p->configContent.clear();
}

void ThreadedHttpPluginServer::Priv::startThreadPool()
{
    if(threadPool->isInitialized())
        return;

    WorkerThreadData::RequestHandlerFactory userFactory = threadPool->requestHandlerFactory();
    WorkerThreadData::CleanupHandlerFactory userCleanup = threadPool->cleanupHandlerFactory();

    ConfigContent content;
    if (!content.load(configFile.file())) {
        qWarning("Tufao::ThreadedHttpPluginServer: Couldn't load new config from"
                 " \"%s\"", qPrintable(configFile.file()));
        return;
    }

    auto factory = [content,userFactory](AbstractConnectionHandler* handler, void **customData){

        HttpServerRequestRouter* router = new HttpServerRequestRouter(handler);

        ThreadData* data = new ThreadData;
        *customData = data;

        HttpPluginServer::Priv::loadNewConfig(router, content,data->plugins,data->handlers);

        if(userFactory)
            userFactory(handler,&data->userCustomData);

        connect(handler,&AbstractConnectionHandler::requestReady,
                router,&HttpServerRequestRouter::handleRequest);
    };

    auto cleanup = [userCleanup](void **customData){
        ThreadData* data = (ThreadData*)*customData;
        if(data){

            //unload all the created handlers
            data->handlers.clear();

            for (const auto &p: data->plugins){
                p->unload();
                //we HAVE to delete now!
                delete p;
            }

            data->plugins.clear();

            if(data->userCustomData){
                if(userCleanup){
                    userCleanup(&data->userCustomData);
                    if(data->userCustomData){
                        qWarning()<<"Tufao::ThreadedHttpPluginServer customData is not cleaned up or not set to 0. Possible memory leak!";
                    }
                }else{
                    qWarning()<<"Tufao::ThreadedHttpPluginServer customData is used but no cleanup function registered! This means leaking memory!";
                }
            }
            delete data;
        }

        *customData = 0;
    };

    threadPool->extStart(threadPool->connectionHandlerFactory(),factory,cleanup);
}

} // namespace Tufao
