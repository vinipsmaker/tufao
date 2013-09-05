#include "priv/threadedhttppluginserver_p.h"
#include "httpserverrequestrouter.h"
#include <QPluginLoader>

namespace Tufao {

AbstractHttpServerRequestHandler *ThreadedHttpPluginServer::defaultFactoy(AbstractHttpServerRequestHandler *handler, void **)
{
    return handler;
}

ThreadedHttpPluginServer::ThreadedHttpPluginServer(Factory fact, QObject *parent)
    : ThreadedHttpRequestDispatcher(new Priv(this),nullptr,parent)
{
    ThreadedHttpPluginServer::Priv* p = ((Priv*)_priv());
    p->factoryFunc = fact;

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
    initializeThreads();

    p->deferredDispatch = false; //start request dispatching
    p->dispatchRequests();

}

void ThreadedHttpPluginServer::initializeThreads()
{
    ThreadedHttpPluginServer::Priv* p = ((Priv*)_priv());

    ThreadedHttpPluginServer::Factory userFactory = p->factoryFunc;
    ThreadedHttpRequestDispatcher::CleanupFunc userCleanup = p->threadCleaner;

    ConfigContent content;
    if (!content.load(p->configFile.file())) {
        qWarning("Tufao::ThreadedHttpPluginServer: Couldn't load new config from"
                 " \"%s\"", qPrintable(p->configFile.file()));
        return;
    }

    auto factory = [content,userFactory](void **customData){

        HttpServerRequestRouter* router = new HttpServerRequestRouter();

        ThreadData* data = new ThreadData;
        *customData = data;

        HttpPluginServer::Priv::loadNewConfig(router, content,data->plugins,data->handlers);

        return userFactory(router,&data->userCustomData);
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

    p->threadListMutex.lock();
    for(unsigned int i = 0; i < p->numberOfThreads; i++){
        WorkerThread *w = new WorkerThread(i,factory,cleanup,p);
        w->start();
        p->idleThreads.append(w);
    }
    p->threadListMutex.unlock();
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

    p->deferredDispatch = true; //stop request dispatching
    p->stopAllThreads(); //shut down all threads

    p->configFile.clear();
    p->configContent.clear();
}

} // namespace Tufao
