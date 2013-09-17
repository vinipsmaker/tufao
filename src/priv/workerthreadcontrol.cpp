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


#include "workerthreadcontrol.h"
#include "threadedhttprequestdispatcher.h"
#include <QThread>
#include <QDebug>

namespace Tufao {

WorkerThreadControl::WorkerThreadControl(QObject *parent) :
    QObject(parent)
{
}

void WorkerThreadControl::cleanup()
{
    //maybe request was deleted while in the eventloop
    if(currentRequest){
        //undo connections
        disconnect(currentRequest.data(),0,this,0);
    }

    if(currentResponse){
        currentResponse->flush();
        disconnect(currentResponse.data(),0,this,0);
    }

    currentRequest.clear();
    currentResponse.clear();
}

void WorkerThreadControl::onRequestReady(HttpServerRequest &request, HttpServerResponse &response)
{
    Q_ASSERT_X(currentRequest.isNull() && currentResponse.isNull(),Q_FUNC_INFO,"Can only handle one request at a time");
    currentRequest = &request;
    currentResponse = &response;

    connect(&request,&HttpServerRequest::close,this,&WorkerThreadControl::onRequestClosed);
    connect(&request,&HttpServerRequest::destroyed,this,&WorkerThreadControl::onRequestDestroyed);
    connect(&response,&HttpServerResponse::finished,this,&WorkerThreadControl::onResponseFinished);
}

void WorkerThreadControl::onResponseFinished()
{
    tDebug()<<"onResponseFinished";

    cleanup();
    QThread::currentThread()->exit();
}

void WorkerThreadControl::onRequestDestroyed()
{
    tDebug()<<"onRequestDestroyed";

    cleanup();
    QThread::currentThread()->exit();
}

void WorkerThreadControl::onRequestClosed()
{
    tDebug()<<"onRequestClosed";

    cleanup();
    QThread::currentThread()->exit();
}

}
