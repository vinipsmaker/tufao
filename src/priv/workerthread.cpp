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
#include <QtCore/QCoreApplication>

#include "../abstractconnectionhandler.h"
#include "../abstracthttpserverrequesthandler.h"

#include "httpserverrequest.h"
#include "httpserverresponse.h"
#include "workerrunnable.h"
#include "workerthreadpool.h"

namespace Tufao {

TUFAO_EXPORT QDebug tDebug ();

WorkerThread::WorkerThread(int id
                           , WorkerThreadData::ConnHandlerFactory connHandlerFactory
                           , WorkerThreadData::RequestHandlerFactory reqHandlerFactory
                           , WorkerThreadData::CleanupHandlerFactory cleanup
                           , WorkerThreadPool *parent)
    : QThread(parent)
    ,id(id)
    ,controller(0)
    ,connHandlerFactory(connHandlerFactory)
    ,reqHandlerFactory(reqHandlerFactory)
    ,cleanup(cleanup)
    ,dispatcher(parent)
{

}

void WorkerThread::handleRequest(qintptr r)
{
    controller->handleConnection(r);
}

void WorkerThread::shutdown()
{
    controller->staticMetaObject.invokeMethod(controller,"onShutdownRequested",Qt::QueuedConnection);
}

WorkerRunnable::Load WorkerThread::workLoad()
{
    return controller->workLoad();
}

void WorkerThread::run()
{
    void* customData = 0; //Custom data storage room

    AbstractConnectionHandler* handler = connHandlerFactory( );
    reqHandlerFactory(handler, &customData);

    controller = new WorkerRunnable(this,handler,handler);
    dispatcher->registerIdleThread(this);

    exec();

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
