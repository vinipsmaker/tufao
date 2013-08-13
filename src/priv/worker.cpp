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

#include "worker.h"

#include <functional>
#include <QtCore/QEventLoop>
#include <QtCore/QRunnable>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QCoreApplication>

#include "../abstracthttpserverrequesthandler.h"
#include "httpserverrequest.h"
#include "httpserverresponse.h"
#include "workerthreadevent.h"
#include "workerthreadcontrol.h"
#include "threadedhttprequestdispatcher.h"

#define debug() qDebug()<<"["<<QThread::currentThreadId()<<"] "

namespace Tufao {

WorkerThread::WorkerThread(int id, std::function<AbstractHttpServerRequestHandler* ()> factory, ThreadedHttpRequestDispatcher *parent)
    : QThread(parent),id(id), factory(factory),dispatcher(parent),shutdownRequested(false)
{

}

void WorkerThread::handleRequest(WorkerThread::Request r)
{
    mutex.lock();

    myRequest = r;

    Q_ASSERT_X(myRequest.response->parent() == myRequest.request
               ,Q_FUNC_INFO
               ,"Response HAS to be a child of Request");

    myRequest.request->moveToThread(this);
    mutex.unlock();

    //wake the thread up
    wait.wakeAll();
}

void WorkerThread::shutdown()
{
    mutex.lock();
    shutdownRequested = true;
    mutex.unlock();
    wait.wakeAll();
}

int WorkerThread::threadId()
{
    return id;
}

void WorkerThread::run()
{
    AbstractHttpServerRequestHandler* handler = factory();

    //Tell the dispatcher we are here
    WorkerThreadEvent* threadEvent = new WorkerThreadEvent(WorkerThreadEvent::ThreadStarted);
    threadEvent->thread = this;
    QCoreApplication::postEvent(dispatcher,threadEvent);

    WorkerThreadControl controller;

    forever{
        //wait for a new request to handle

        debug()<<"Goes to sleep";

        mutex.lock();
        wait.wait(&mutex);

        if(shutdownRequested){
            mutex.unlock();
            break;
        }
        mutex.unlock();

        debug()<<"Woke up";

        //Tell the dispatcher we are working
        WorkerThreadEvent* threadEvent = new WorkerThreadEvent(WorkerThreadEvent::ThreadRunning);
        threadEvent->thread = this;
        QCoreApplication::postEvent(dispatcher,threadEvent);

        HttpServerRequest& request   = *myRequest.request;
        HttpServerResponse& response = *myRequest.response;

        //make shure eventloop is exited when the request is done
        connect(myRequest.request.data(),&HttpServerRequest::close,&controller,&WorkerThreadControl::onRequestClosed);
        connect(myRequest.request.data(),&HttpServerRequest::destroyed,&controller,&WorkerThreadControl::onRequestDestroyed);
        connect(myRequest.response.data(),&HttpServerResponse::finished,&controller,&WorkerThreadControl::onResponseFinished);

        bool handled = handler->handleRequest(request,response);



        //If the request was handled the user should have called end()
        if(!handled){
            //our request was not handled who needs to clean it up?
            request.end();
        }

        debug()<<"Enters Eventloop";
        /*enter eventloop in case the request is handled async
         *or needs to be deleted
         */
        exec();

        debug()<<"Leaves Eventloop";

        //Tell the dispatcher we are done
        threadEvent = new WorkerThreadEvent(WorkerThreadEvent::ThreadIdle);
        threadEvent->thread = this;

        //maybe request was deleted while in the eventloop
        if(myRequest.request){
            //undo connections
            disconnect(myRequest.request.data(),0,&controller,0);

            if(myRequest.response)
                disconnect(myRequest.response.data(),0,&controller,0);

            //push request object back to the dispatcher's thread
            request.moveToThread(dispatcher->thread());

        }

        QCoreApplication::postEvent(dispatcher,threadEvent);

    }

    debug()<<"!!!!!!!!!!!!THREAD GOES DOWN";
    //clean up all ressources
    delete handler;

}

} //namespace Tufao
