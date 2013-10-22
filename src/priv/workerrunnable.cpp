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

#include "workerrunnable.h"
#include <QTimer>
#include <QDateTime>
#include <QMutexLocker>
#include <QThread>

#include "../abstractconnectionhandler.h"
#include "../httpserverrequest.h"
#include "../httpserverresponse.h"
#include "workerthread.h"
#include "workerthreadpool.h"

namespace Tufao {

TUFAO_EXPORT QDebug tDebug ();

WorkerRunnable::WorkerRunnable(WorkerThread* thread,AbstractConnectionHandler* handler, QObject *parent)
    : QObject(parent)
    , thread(thread)
    , connHandler(handler)
    , lastResponseTimeout(QDateTime::currentDateTime().toMSecsSinceEpoch())
    , responsiveTimer(new QTimer(this))
    , currentRequestCount(0)

{
    connect(responsiveTimer,&QTimer::timeout,this,&WorkerRunnable::onResponsiveTimer);

    //make sure we know everything about request states
    connect(connHandler,&AbstractConnectionHandler::newRequest  ,this,&WorkerRunnable::onNewRequest);

    responsiveTimer->setInterval(100);
    responsiveTimer->start();

}

WorkerRunnable::Load WorkerRunnable::workLoad() const
{
    lastResponseMutex.lock();

    quint64 now  = QDateTime::currentDateTime().toMSecsSinceEpoch();
    quint64 diff = now - lastResponseTimeout;

    lastResponseMutex.unlock();

    if(diff < 150)
        return Low;
    else if (diff < 500)
        return Medium;

    return High;
}

void WorkerRunnable::handleConnection(qintptr socketDesc)
{
    QMutexLocker l(&queueMutex);
    pendingConnections.enqueue(socketDesc);

    staticMetaObject.invokeMethod(this,"flushQueue",Qt::QueuedConnection);
}

void WorkerRunnable::onShutdownRequested()
{
    while(pendingConnections.size()){
        int socketDesc = pendingConnections.dequeue();
        connHandler->closePendingConnection(socketDesc);
    }
    QThread::currentThread()->exit();
}

//only called from within workerthread
void WorkerRunnable::flushQueue()
{
    QMutexLocker l(&queueMutex);

    bool wasIdle = (currentRequestCount == 0);

    while(pendingConnections.size()){
        int socketDesc = pendingConnections.dequeue();
        connHandler->incomingConnection(socketDesc);
    }

    if(currentRequestCount == 0)
        thread->pool()->registerIdleThread(thread);
    else{
        //make sure we are not marked idle
        if(wasIdle)
            thread->pool()->unregisterIdleThread(thread);
    }
}

void WorkerRunnable::onResponsiveTimer()
{
    QMutexLocker l(&lastResponseMutex);
    lastResponseTimeout = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void WorkerRunnable::onNewRequest(HttpServerRequest &request)
{
    currentRequestCount++;
    connect(&request,&HttpServerRequest::ready,this,&WorkerRunnable::onRequestReady);
    connect(&request,&HttpServerRequest::destroyed,this,&WorkerRunnable::onRequestDestroyed);
}

void WorkerRunnable::onRequestReady()
{
    connect(sender(),SIGNAL(end()),this,SLOT(onRequestEnd()));
    tDebug()<<"onRequestReady";
}

void WorkerRunnable::onRequestEnd()
{
    tDebug()<<"onRequestEnd";
}

void WorkerRunnable::onRequestDestroyed()
{
    currentRequestCount--;

    tDebug()<<"onRequestDestroyed Open Requests: "<<currentRequestCount;

    if(currentRequestCount == 0)
        thread->pool()->registerIdleThread(thread);
}

} // namespace Tufao
