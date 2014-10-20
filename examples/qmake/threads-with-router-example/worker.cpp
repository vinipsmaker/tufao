#include "worker.h"

#include <QtNetwork/QTcpSocket>

#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

using namespace Tufao;

Worker::Worker()
{
    connect(this, SIGNAL(initReady()), this, SLOT(init()),
            Qt::QueuedConnection);
    connect(this, &Worker::newConnection/*SIGNAL(newConnection(qintptr))*/,
            this, &Worker::onNewConnection/*SLOT(onNewConnection(qintptr))*/,
            Qt::QueuedConnection);
}

void Worker::setFactory(TcpServer::Factory factory)
{
    factoryMutex.lock();
    this->factory = factory;
    factoryMutex.unlock();
    emit initReady();
}

void Worker::addConnection(qintptr socketDescriptor)
{
    emit newConnection(socketDescriptor);
}

void Worker::init()
{
    factoryMutex.lock();
    handler = factory();
    factoryMutex.unlock();
}

void Worker::onNewConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    HttpServerRequest *handle = new HttpServerRequest(*socket, this);

    connect(handle, SIGNAL(ready()), this, SLOT(onRequestReady()));
    connect(handle, SIGNAL(upgrade()), this, SLOT(onUpgrade()));
    connect(socket, SIGNAL(disconnected()), handle, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Worker::onRequestReady()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());

    QAbstractSocket &socket = request->socket();
    HttpServerResponse *response
            = new HttpServerResponse(socket, request->responseOptions(), this);

    connect(&socket, SIGNAL(disconnected()), response, SLOT(deleteLater()));
    connect(response, SIGNAL(finished()), response, SLOT(deleteLater()));

    if (request->headers().contains("Expect", "100-continue"))
        response->writeContinue();

    handler(*request, *response);
}

void Worker::onUpgrade()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    delete request;
}
