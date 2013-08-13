/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

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
#include <QMutex>
#include <QWaitCondition>
#include <QPointer>
#include <functional>



namespace Tufao {

class ThreadedHttpRequestDispatcher;
class HttpServerRequest;
class HttpServerResponse;
class AbstractHttpServerRequestHandler;

class WorkerThread : public QThread
{
    Q_OBJECT

    public:

        struct Request{
            QPointer<HttpServerRequest>  request;
            QPointer<HttpServerResponse> response;
        };

        WorkerThread(int id, std::function<AbstractHttpServerRequestHandler* ()> factory, ThreadedHttpRequestDispatcher* parent);

        void handleRequest(Request r);
        void shutdown     ();
        int  threadId     ();


    // QThread interface
    protected:
        virtual void run();

    private:
        int    id;
        QMutex mutex;
        QWaitCondition wait; //the worker thread is sleeping on this waitcondition
        Request myRequest; //the request that is handled by this thread
        bool shutdownRequested;

        std::function<AbstractHttpServerRequestHandler* ()> factory;
        ThreadedHttpRequestDispatcher* dispatcher;
};


} //namespace Tufao


#endif //TUFAO_PRIV_WORKERTHREAD_H
