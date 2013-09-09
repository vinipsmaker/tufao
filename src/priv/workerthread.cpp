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
#include <QtNetwork/QAbstractSocket>

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
    QCoreApplication::postEvent(dispatcher->pub,new WorkerThreadEvent(WorkerThreadEvent::ThreadStarted,this));

    WorkerThreadControl controller;

    //lock the mutex only in the first run, later the mutex will be locked
    //at the end of the loop
    mutex.lock();
    forever{

        tDebug()<<"Goes to sleep";
        m_wait.wait(&mutex);

        if(shutdownRequested){
            mutex.unlock();
            break;
        }else{
            tDebug()<<"Woke up";

            QPointer<HttpServerRequest>  request  = myRequest.request;
            QPointer<HttpServerResponse> response = myRequest.response;

            myRequest.request.clear();
            myRequest.response.clear();

            mutex.unlock();

            //deliver all events to the internal socket
            //QCoreApplication::sendPostedEvents(&request->socket());

            //Tell the dispatcher we are working
            QCoreApplication::postEvent(dispatcher->pub,new WorkerThreadEvent(WorkerThreadEvent::ThreadRunning,this));

            //make sure eventloop is exited when the request is done
            connect(request.data(),&HttpServerRequest::close,&controller,&WorkerThreadControl::onRequestClosed);
            connect(request.data(),&HttpServerRequest::destroyed,&controller,&WorkerThreadControl::onRequestDestroyed);
            connect(response.data(),&HttpServerResponse::finished,&controller,&WorkerThreadControl::onResponseFinished);

            bool handled = handler->handleRequest(*request,*response);


            //If the request was handled the user should have called end()
            if(!handled){
                //our request was not handled who needs to clean it up?
                request->end();
            }

            tDebug()<<"Enters Eventloop";
            /*enter eventloop in case the request is handled async
             *or needs to be deleted
             */
            exec();

            tDebug()<<"Leaves Eventloop";

            //maybe request was deleted while in the eventloop
            if(request){
                //undo connections
                disconnect(request.data(),0,&controller,0);

                if(response){
                    response->flush();
                    disconnect(response.data(),0,&controller,0);
                }

                //push request object back to the dispatcher's thread
                request->moveToThread(dispatcher->pub->thread());

            }

            mutex.lock();
            dispatcher->takeWorkingThread(this);

            //mutex is unlocked after we went to sleep
        }
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
