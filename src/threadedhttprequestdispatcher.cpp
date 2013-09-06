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
    : numberOfThreads (10), deferredDispatch(false),rejectRequests(false), pub(pub)
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

void ThreadedHttpRequestDispatcher::Priv::dispatchRequests()
{
    if(deferredDispatch)
        return;

    qDebug()<<"Starting request dispatch";
    while(pendingRequests.size()){

        WorkerThread* w = takeIdleThread();
        if(!w){
            qDebug()<<"!!!!!!!!!!! We ran out of threads !!!!!!!!!!!!!!!!!!!!!!!!!!!";
            break;
        }

        WorkerThread::Request r = pendingRequests.dequeue();

        //maybe the request was deleted while waiting (high serverload?)
        if(r.request && r.response)
            w->handleRequest(r);
        else{
            if(r.request)
                r.request->deleteLater();
            if(r.response)
                r.response->deleteLater();
        }
    }
    qDebug()<<"Finished request dispatch";
}

void ThreadedHttpRequestDispatcher::Priv::stopAllThreads()
{
    QTime timer;
    long timeout = 60 * 1000;

    timer.start();

    while(true){

        int size = 0;
        threadListMutex.lock();
        size = workingThreads.size();
        threadListMutex.unlock();

        if(size <= 0)
            break;

        //deliver thread finished events to us
        //QCoreApplication::sendPostedEvents(pub,WorkerThreadEvent::ThreadIdle);
        QThread::msleep(10);

        /* If we hit this, we waited long enough, maybe the threads
         * are somehow blocked. We need to force quit them
         */
        if(timer.elapsed() > timeout){
            threadListMutex.lock();
            for(auto i = workingThreads.begin();
                i != workingThreads.end(); i++){

                WorkerThread* th = i.value();
                th->terminate();
                th->wait(10*1000);
                delete th;

            }
            workingThreads.clear();
            threadListMutex.unlock();
            break;
        }

    }

    timeout = 10 * 1000;

    //quit all threads
    QMutexLocker l(&threadListMutex);
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

WorkerThread *ThreadedHttpRequestDispatcher::Priv::takeIdleThread()
{
    QMutexLocker l(&threadListMutex);

    if(idleThreads.size()){
        WorkerThread* wt = idleThreads.takeFirst();
        workingThreads.insert(wt->threadId(),wt);

        qDebug()<<"Idle Threads: "<<idleThreads.size();
        qDebug()<<"Working Threads: "<<workingThreads.size()<<" "<<workingThreads.keys();

        return wt;
    }
    return 0;
}

void ThreadedHttpRequestDispatcher::Priv::takeWorkingThread(WorkerThread *thread)
{
    QMutexLocker l(&threadListMutex);
    if(!workingThreads.contains(thread->threadId())){
        qDebug()<<"Thread ["<<thread->threadId()<<"] goes into idle mode but is not in working Map";
        return;
    }
    idleThreads.append(workingThreads.take(thread->threadId()));

    qDebug()<<"Thread ["<<thread->threadId()<<"] finished work";
    qDebug()<<"Idle Threads: "<<idleThreads.size();
    qDebug()<<"Working Threads: "<<workingThreads.size()<<" "<<workingThreads.keys();

    l.unlock();

    QCoreApplication::postEvent(pub,new WorkerThreadEvent(WorkerThreadEvent::ThreadIdle,thread));
}

bool ThreadedHttpRequestDispatcher::ThreadedHttpRequestDispatcher::event(QEvent * e)
{
    if(e->type() == WorkerThreadEvent::ThreadIdle){
        if(priv->pendingRequests.size())
            priv->dispatchRequests();
        return true;
    }
    return QObject::event(e);
}

QDebug tDebug()
{
    WorkerThread* t = qobject_cast<WorkerThread*>(QThread::currentThread());
    if(t)
        return qDebug()<<"["<<t->threadId()<<"]";

    return qDebug();
}

} // namespace Tufao

