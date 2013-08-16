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

#include "priv/worker.h"
#include "priv/workerthreadevent.h"

#include <algorithm>
#include <QEvent>
#include <QTime>

namespace Tufao {

ThreadedHttpRequestDispatcher::Priv::Priv()
    : numberOfThreads (10), deferredDispatch(false)
{

}


ThreadedHttpRequestDispatcher::ThreadedHttpRequestDispatcher(Factory threadInitializer, QObject *parent) :
    QObject(parent),
    priv(new Priv)
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
    priv->dispatchRequests();
}

void ThreadedHttpRequestDispatcher::shutdown()
{
    priv->deferredDispatch = true; //stop request dispatching

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
    WorkerThread::Request r;
    r.request  = &request;
    r.response = &response;

    priv->pendingRequests.append(r);
    if(priv->idleThreads.size())
        priv->dispatchRequests();

    return true;
}

void ThreadedHttpRequestDispatcher::initializeThreads()
{
    for(unsigned int i = 0; i < priv->numberOfThreads; i++){
        WorkerThread *w = new WorkerThread(i,priv->threadInitializer,this);
        w->start();
        priv->idleThreads.append(w);
    }
}

void ThreadedHttpRequestDispatcher::Priv::dispatchRequests()
{
    if(deferredDispatch)
        return;

    while(pendingRequests.size()){
        if(idleThreads.size() == 0)
            break;

        WorkerThread* w = idleThreads.takeFirst();
        workingThreads.insert(w->threadId(),w);

        WorkerThread::Request r = pendingRequests.dequeue();
        w->handleRequest(r);
    }
}

void ThreadedHttpRequestDispatcher::Priv::stopAllThreads()
{
    QTime timer;
    long timeout = 60 * 1000;

    timer.start();

    while(workingThreads.size()){
        //deliver thread finished events to us
        QCoreApplication::sendPostedEvents(this,WorkerThreadEvent::ThreadIdle);
        QThread::msleep(10);

        /* If we hit this, we waited long enough, maybe the threads
         * are somehow blocked. We need to force quit them
         */
        if(timer.elapsed() > timeout){
            for(auto i = workingThreads.begin();
                i != workingThreads.end(); i++){

                WorkerThread* th = i.value();
                th->terminate();
                th->wait(10*1000);
                delete th;

            }
            workingThreads.clear();
            break;
        }

    }

    timeout = 10 * 1000;

    //quit all threads
    for(int i= 0; i < idleThreads.size(); i++){
        WorkerThread* th = idleThreads.takeAt(i);
        th->shutdown();
        bool finished = th->wait(timeout);
        if(!finished){
            th->terminate();
            finished = th->wait(timeout);

            //@BUG what to do if the thread can not be terminated
            if(!finished)
                tDebug()<<"Could not stop thread correctly";
        }
        delete th;
    }
}

bool ThreadedHttpRequestDispatcher::ThreadedHttpRequestDispatcher::event(QEvent * e)
{
    switch(e->type()){
        case (QEvent::Type) WorkerThreadEvent::ThreadStarted:{
            WorkerThreadEvent *event = static_cast<WorkerThreadEvent*>(e);
            qDebug()<<"Thread ["<<event->thread->threadId()<<"] up and running";

            return true;
        }
        case (QEvent::Type)WorkerThreadEvent::ThreadIdle:{
            WorkerThreadEvent *event = static_cast<WorkerThreadEvent*>(e);
            if(!priv->workingThreads.contains(event->thread->threadId())){
                qDebug()<<"Thread ["<<event->thread->threadId()<<"] goes into idle mode but is not in working Map";
            }else{
                WorkerThread* th = priv->workingThreads.take(event->thread->threadId());
                priv->idleThreads.append(th);

                qDebug()<<"Thread ["<<event->thread->threadId()<<"] finished work";
                qDebug()<<"Idle Threads: "<<priv->idleThreads.size();
                qDebug()<<"Working Threads: "<<priv->workingThreads.size()<<" "<<priv->workingThreads.keys();

                if(priv->idleThreads.size() && priv->pendingRequests.size())
                    priv->dispatchRequests();

            }

            return true;
        }
        case (QEvent::Type) WorkerThreadEvent::ThreadRunning:{
            WorkerThreadEvent *event = static_cast<WorkerThreadEvent*>(e);
            qDebug()<<"Thread "<<event->thread->threadId()<<" started to work";

            return true;
        }
        default:
            return QObject::event(e);
    }
}

QDebug tDebug()
{
    WorkerThread* t = qobject_cast<WorkerThread*>(QThread::currentThread());
    if(t)
        return qDebug()<<"["<<t->threadId()<<"]";

    return qDebug();
}

} // namespace Tufao

