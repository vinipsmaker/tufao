/*  This file is part of the Tufão project
    Copyright (C) 2013 Benjamin Zeller <zeller.benjamin@web.de>

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

#include "priv/threadedhttpserver_p.h"
#include <QScopedPointer>
#include <QTimer>

namespace Tufao {

ThreadedHttpServer::ConnectionHandlerFactory ThreadedHttpServer::Priv::defConnHdl = [] (){
    return new HttpConnectionHandler();
};

ThreadedHttpServer::Priv::Priv()
    : threadPool(new WorkerThreadPool)
    , rejectRequests(false)
    , pub(0)
{
}

void ThreadedHttpServer::Priv::init(ThreadedHttpServer *pub)
{
    this->pub = pub;
    threadPool->setParent(pub);

    QObject::connect(&tcpServer, &TcpServerWrapper::newConnection,
                    pub, &ThreadedHttpServer::onNewConnection);

    threadPool->setConnectionHandlerFactory(defaultConnectionHandlerFactory());
}
void ThreadedHttpServer::Priv::startThreadPool()
{
    threadPool->start();
}

ThreadedHttpServer::ConnectionHandlerFactory ThreadedHttpServer::defaultConnectionHandlerFactory()
{
    return Priv::defConnHdl;
}

ThreadedHttpServer::ThreadedHttpServer(Priv *priv, QObject *parent)
    : QObject(parent),priv(priv)
{
    priv->init(this);
}

ThreadedHttpServer::ThreadedHttpServer(QObject *parent)
    : ThreadedHttpServer(new Priv,parent)
{
    QTimer::singleShot(0,priv->threadPool,SLOT(start()));
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

    priv->startThreadPool();

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

ThreadedHttpServer::Priv *ThreadedHttpServer::_priv()
{
    return priv;
}

const ThreadedHttpServer::Priv *ThreadedHttpServer::_priv() const
{
    return priv;
}


QDebug tDebug()
{
    WorkerThread* t = qobject_cast<WorkerThread*>(QThread::currentThread());
    if(t)
        return qDebug()<<"["<<t->threadId()<<"]";

    return qDebug();
}

} // namespace Tufao
