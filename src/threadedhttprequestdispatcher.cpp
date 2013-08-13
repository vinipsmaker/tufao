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

#include "priv/threadedhttprequestdispatcher.h"
#include "httpserverrequest.h"

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QDebug>

#include "priv/worker.h"
#include "priv/workerthreadevent.h"

#include <algorithm>
#include <QEvent>

namespace Tufao {

ThreadedHttpRequestDispatcher::ThreadedHttpRequestDispatcher(Factory threadInitializer, QObject *parent) :
    QObject(parent),
    priv(new Priv)
{
    priv->threadInitializer = threadInitializer;

    //activate threads when the eventloop is started, to give the user a chance
    //to set some default values
    QMetaObject::invokeMethod(this,"initializeThreads",Qt::QueuedConnection);
    //push mappings into the thread, so it can create the handlers itself
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
    /*
    //quit all threads
    for(int i= 0; i < priv->workers.size(); i++){
        //@BUG need a better way to do this
        priv->workers[i]->terminate();
        priv->workers[i]->deleteLater();
    }
    priv->workers.clear();
    initializeThreads();
    */
}


bool ThreadedHttpRequestDispatcher::handleRequest(HttpServerRequest &request,
                                            HttpServerResponse &response)
{

    qDebug()<<"Incoming Request";
    WorkerThread::Request r;
    r.request  = &request;
    r.response = &response;

    priv->pendingRequests.append(r);
    if(priv->idleThreads.size())
        priv->scheduleRequests();

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

void ThreadedHttpRequestDispatcher::Priv::scheduleRequests()
{
    while(pendingRequests.size()){
        if(idleThreads.size() == 0)
            break;

        WorkerThread* w = idleThreads.takeFirst();
        workingThreads.insert(w->threadId(),w);

        WorkerThread::Request r = pendingRequests.dequeue();
        w->handleRequest(r);
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
                    priv->scheduleRequests();

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

} // namespace Tufao

