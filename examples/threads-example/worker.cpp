#include "worker.h"

#include <QtNetwork/QTcpSocket>

#include <Tufao/AbstractHttpServerRequestHandler>
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

using namespace Tufao;

Worker::Worker(Tufao::AbstractHttpServerRequestHandlerFactory *factory) :
    handler(factory->createHandler(this))
{
    connect(this, SIGNAL(newConnection(int)), this, SLOT(onNewConnection(int)));
}

void Worker::addConnection(int socketDescriptor)
{
    emit newConnection(socketDescriptor);
}


void Worker::onNewConnection(int socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    HttpServerRequest *handle = new HttpServerRequest(socket, this);

    connect(handle, SIGNAL(ready()), this, SLOT(onRequestReady()));
    connect(handle, SIGNAL(upgrade(QByteArray)),
            this, SLOT(onUpgrade(QByteArray)));
    connect(socket, SIGNAL(disconnected()), handle, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Worker::onRequestReady()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());

    QAbstractSocket *socket = request->socket();
    HttpServerResponse *response
            = new HttpServerResponse(socket, request->responseOptions(), this);

    connect(socket, SIGNAL(disconnected()), response, SLOT(deleteLater()));
    connect(response, SIGNAL(finished()), response, SLOT(deleteLater()));

    if (request->headers().contains("Expect", "100-continue"))
        response->writeContinue();

    handler->handleRequest(request, response);
}

void Worker::onUpgrade(const QByteArray &)
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    delete request;
}
