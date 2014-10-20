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

#include "tcpserver.h"
#include "worker.h"

#include <QThread>

using namespace Tufao;

TcpServer::TcpServer(QObject *parent) :
    QTcpServer(parent),
    i(0)
{
}

void TcpServer::run(int threadsNumber, int port, Factory factory)
{
    workers.reserve(threadsNumber);
    for (int i = 0;i != threadsNumber;++i) {
        Worker *worker = new Worker;
        QThread *workerThread = new QThread(this);

        worker->moveToThread(workerThread);
        workerThread->start();

        worker->setFactory(factory);

        workers.push_back(worker);
    }
    listen(QHostAddress::Any, port);
}

void TcpServer::incomingConnection(qintptr handle)
{
    workers[(i++) % workers.size()]->addConnection(handle);
}
