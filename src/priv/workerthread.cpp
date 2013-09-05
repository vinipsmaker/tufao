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

#include "workerthread.h"

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

namespace Tufao {

WorkerThread::WorkerThread(int id,
                           std::function<AbstractHttpServerRequestHandler* (void **)> factory,
                           std::function<void (void **customData)> cleanup,
                           ThreadedHttpRequestDispatcher::Priv *parent)
    : QThread(parent->pub),id(id),shutdownRequested(false), factory(factory),cleanup(cleanup),dispatcher(parent)
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
    myRequest.response->moveToThread(this);
    mutex.unlock();

    //wake the thread up
    m_wait.wakeAll();
}

void WorkerThread::shutdown()
{
    mutex.lock();
    shutdownRequested = true;
    mutex.unlock();
    m_wait.wakeAll();
}

int WorkerThread::threadId()
{
    return id;
}

void WorkerThread::run()
{
    void* customData = 0; //Custom data storage room
    AbstractHttpServerRequestHandler* handler = factory(&customData);

    //Tell the dispatcher we are here
    WorkerThreadEvent* threadEvent = new WorkerThreadEvent(WorkerThreadEvent::ThreadStarted);
    threadEvent->thread = this;
    QCoreApplication::postEvent(dispatcher->pub,threadEvent);

    WorkerThreadControl controller;

    forever{
        //wait for a new request to handle

        tDebug()<<"Goes to sleep";

        mutex.lock();
        m_wait.wait(&mutex);

        if(shutdownRequested){
            mutex.unlock();
            break;
        }
        mutex.unlock();

        tDebug()<<"Woke up";

        //Tell the dispatcher we are working
        WorkerThreadEvent* threadEvent = new WorkerThreadEvent(WorkerThreadEvent::ThreadRunning);
        threadEvent->thread = this;
        QCoreApplication::postEvent(dispatcher->pub,threadEvent);

        HttpServerRequest& request   = *myRequest.request;
        HttpServerResponse& response = *myRequest.response;

        //make sure eventloop is exited when the request is done
        connect(myRequest.request.data(),&HttpServerRequest::close,&controller,&WorkerThreadControl::onRequestClosed);
        connect(myRequest.request.data(),&HttpServerRequest::destroyed,&controller,&WorkerThreadControl::onRequestDestroyed);
        connect(myRequest.response.data(),&HttpServerResponse::finished,&controller,&WorkerThreadControl::onResponseFinished);

        bool handled = handler->handleRequest(request,response);


        //If the request was handled the user should have called end()
        if(!handled){
            //our request was not handled who needs to clean it up?
            request.end();
        }

        tDebug()<<"Enters Eventloop";
        /*enter eventloop in case the request is handled async
         *or needs to be deleted
         */
        exec();

        tDebug()<<"Leaves Eventloop";

        //Tell the dispatcher we are done
        threadEvent = new WorkerThreadEvent(WorkerThreadEvent::ThreadIdle);
        threadEvent->thread = this;

        //maybe request was deleted while in the eventloop
        if(myRequest.request){
            //undo connections
            disconnect(myRequest.request.data(),0,&controller,0);

            if(myRequest.response){
                myRequest.response->flush();
                disconnect(myRequest.response.data(),0,&controller,0);
            }

#if 1
            //push request object back to the dispatcher's thread
            request.moveToThread(dispatcher->pub->thread());
#endif

        }

        //make the events a very high priority so they are always delivered
        //QCoreApplication::postEvent(dispatcher->pub,threadEvent,1000);
        mutex.lock();
        dispatcher->takeWorkingThread(this);
        mutex.unlock();

    }

    tDebug()<<"!!!!!!!!!!!!THREAD GOES DOWN";
    //clean up all ressources
    if(customData){
        if(cleanup){
            cleanup(&customData);
            if(customData){
                qWarning()<<"Tufao::ThreadedHttpRequestDispatcher customData is not cleaned up or not set to 0. Possible data leak!";
            }
        }else{
            qWarning()<<"Tufao::ThreadedHttpRequestDispatcher customData is used but no cleanup function registered! This means leaking Data!";
        }
    }
    delete handler;

}

} //namespace Tufao
