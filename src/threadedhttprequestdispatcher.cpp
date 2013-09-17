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

#include "priv/threadedhttprequestdispatcher.h"
#include "httpserverrequest.h"

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include "priv/workerthread.h"
#include "priv/workerthreadevent.h"

#include <algorithm>
#include <QEvent>
#include <QTime>

namespace Tufao {

ThreadedHttpRequestDispatcher::Priv::Priv(ThreadedHttpRequestDispatcher *pub)
    : numberOfThreads (10), deferredDispatch(false),rejectRequests(false), pub(pub),maxParallelThreads(0)
{

}

ThreadedHttpRequestDispatcher::ThreadedHttpRequestDispatcher(Factory threadInitializer, QObject *parent) :
    QObject(parent),
    priv(new Priv(this))
{
    priv->threadInitializer = threadInitializer;

    //activate threads when the eventloop is started, to give the user a chance
    //to set some default values
    QMetaObject::invokeMethod(this,"initializeThreads",Qt::QueuedConnection);
}

ThreadedHttpRequestDispatcher::~ThreadedHttpRequestDispatcher()
{
    delete priv;
}

void ThreadedHttpRequestDispatcher::setCleanupFunction(ThreadedHttpRequestDispatcher::CleanupFunc fun)
{
    priv->threadCleaner = fun;
}

void ThreadedHttpRequestDispatcher::setThreadPoolSize(const unsigned int size)
{
    priv->numberOfThreads  = size;
}

void ThreadedHttpRequestDispatcher::restart()
{
    priv->deferredDispatch = true; //stop request dispatching

    priv->stopAllThreads();
    initializeThreads();

    //give the threads some time to start
    QThread::msleep(100);
    priv->deferredDispatch = false;
    priv->rejectRequests   = false;

    priv->dispatchRequests();
}

void ThreadedHttpRequestDispatcher::shutdown()
{
    priv->rejectRequests = true; //stop request dispatching

    while(priv->pendingRequests.size()){
        WorkerThread::Request r = priv->pendingRequests.dequeue();
        r.response->writeHead(HttpResponseStatus::SERVICE_UNAVAILABLE);
        r.response->end();
    }

    priv->stopAllThreads();
}


bool ThreadedHttpRequestDispatcher::handleRequest(HttpServerRequest &request,
                                            HttpServerResponse &response)
{
    if(priv->rejectRequests)
        return false;

    qDebug()<<"Handling request";

    WorkerThread::Request r;
    r.request  = &request;
    r.response = &response;

    priv->pendingRequests.append(r);
    priv->dispatchRequests();

    return true;
}

void ThreadedHttpRequestDispatcher::initializeThreads()
{
    priv->threadListMutex.lock();
    for(unsigned int i = 0; i < priv->numberOfThreads; i++){
        WorkerThread *w = new WorkerThread(i,priv->threadInitializer,priv->threadCleaner,priv);
        w->start();
        priv->idleThreads.append(w);
    }
    priv->threadListMutex.unlock();
}

ThreadedHttpRequestDispatcher::ThreadedHttpRequestDispatcher(ThreadedHttpRequestDispatcher::Priv *priv, ThreadedHttpRequestDispatcher::Factory threadInitializer, QObject *parent)
    :QObject(parent),priv(priv)
{
    priv->threadInitializer = threadInitializer;

    //do not activate threads automatically subclasses will do that
}

ThreadedHttpRequestDispatcher::Priv *ThreadedHttpRequestDispatcher::_priv()
{
    return priv;
}

const ThreadedHttpRequestDispatcher::Priv *ThreadedHttpRequestDispatcher::_priv() const
{
    return priv;
}



} // namespace Tufao

