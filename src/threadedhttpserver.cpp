#include "priv/threadedhttpserver_p.h"
#include <QScopedPointer>

namespace Tufao {

ThreadedHttpServer::ConnectionHandlerFactory ThreadedHttpServer::Priv::defConnHdl = [] (){
    return new HttpConnectionHandler();
};

ThreadedHttpServer::Priv::Priv(ThreadedHttpServer *pub)
    : threadPool(new WorkerThreadPool(pub))
    , rejectRequests(false)
    , pub(pub)
{
    threadPool->setConnectionHandlerFactory(defaultConnectionHandlerFactory());
}

ThreadedHttpServer::ConnectionHandlerFactory ThreadedHttpServer::defaultConnectionHandlerFactory()
{
    return Priv::defConnHdl;
}

ThreadedHttpServer::ThreadedHttpServer(QObject *parent)
    : QObject(parent),priv(new Priv(this))
{
    connect(&priv->tcpServer, &TcpServerWrapper::newConnection,
            this, &ThreadedHttpServer::onNewConnection);
}

ThreadedHttpServer::~ThreadedHttpServer()
{
    delete priv;
}

bool ThreadedHttpServer::listen(const QHostAddress &address, quint16 port)
{
    return priv->tcpServer.listen(address, port);
}

bool ThreadedHttpServer::isListening() const
{
    return priv->tcpServer.isListening();
}

quint16 ThreadedHttpServer::serverPort() const
{
    return priv->tcpServer.serverPort();
}

void ThreadedHttpServer::setConnectionHandlerFactory(ThreadedHttpServer::ConnectionHandlerFactory factory)
{
    priv->threadPool->setConnectionHandlerFactory(factory);
}

void ThreadedHttpServer::setRequestHandlerFactory(ThreadedHttpServer::RequestHandlerFactory factory)
{
    priv->threadPool->setRequestHandlerFactory(factory);
}

void ThreadedHttpServer::close()
{
    priv->tcpServer.close();
}

void ThreadedHttpServer::setCleanupHandlerFactory(ThreadedHttpServer::CleanupHandlerFactory fun)
{
    priv->threadPool->setCleanupHandlerFactory(fun);
}

void ThreadedHttpServer::setThreadPoolSize(const unsigned int size)
{
    priv->threadPool->setPoolSize(size);
}

void ThreadedHttpServer::restart()
{
    WorkerThreadPool* pool = priv->threadPool;

    pool->pauseDispatch(true);
    pool->stopAllThreads();
    pool->startThreads();

    priv->rejectRequests   = false;
    pool->pauseDispatch(false);
}

void ThreadedHttpServer::shutdown()
{
    priv->rejectRequests = true; //stop request dispatching
    priv->threadPool->shutdown();
}

void ThreadedHttpServer::onNewConnection(qintptr socketDescriptor)
{
    if(priv->rejectRequests){
        QScopedPointer<AbstractConnectionHandler> handler(priv->threadPool->connectionHandlerFactory()());
        handler->closePendingConnection(socketDescriptor);
        return;
    }

    priv->threadPool->handleConnection(socketDescriptor);
}


QDebug tDebug()
{
    WorkerThread* t = qobject_cast<WorkerThread*>(QThread::currentThread());
    if(t)
        return qDebug()<<"["<<t->threadId()<<"]";

    return qDebug();
}

} // namespace Tufao
