/* This file is part of the Tufão project
   Copyright (c) 2013 Benjamin Zeller

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

#ifndef TUFAO_PRIV_WORKERTHREAD_H
#define TUFAO_PRIV_WORKERTHREAD_H

#include <QtCore/QObject>

#include <QThread>
#include <functional>
#include <QDebug>

#include "workerrunnable.h"
#include "workerthreadpool.h"

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;
class AbstractConnectionHandler;
class WorkerRunnable;
class WorkerThreadPool;
class AbstractHttpServerRequestHandler;

class WorkerThread : public QThread
{
    Q_OBJECT

    public:
        WorkerThread(int id
                     ,WorkerThreadData::ConnHandlerFactory connHandlerFactory
                     ,WorkerThreadData::RequestHandlerFactory reqHandlerfactory
                     ,WorkerThreadData::CleanupHandlerFactory cleanup
                     ,WorkerThreadPool* parent);

        void handleRequest   (qintptr r);
        void shutdown     ();
        WorkerRunnable::Load workLoad         ();

        inline int  threadId     (){
            return id;
        }

        inline WorkerThreadPool *pool(){
            return dispatcher;
        }


    // QThread interface
    protected:
        virtual void run();

    private:
        int    id;

        WorkerRunnable* controller;

        WorkerThreadData::ConnHandlerFactory    connHandlerFactory;
        WorkerThreadData::RequestHandlerFactory reqHandlerFactory;
        WorkerThreadData::CleanupHandlerFactory cleanup;
        WorkerThreadPool* dispatcher;
};

} //namespace Tufao


#endif //TUFAO_PRIV_WORKERTHREAD_H
