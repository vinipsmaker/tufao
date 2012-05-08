/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "priv/httpserver.h"
#include "httpserverrequest.h"
#include <QtNetwork/QTcpSocket>
#include "headers.h"

namespace Tufao {

HttpServer::HttpServer(QObject *parent) :
    QObject(parent),
    priv(new Priv)
{
    connect(&priv->tcpServer, SIGNAL(newConnection(int)),
            this, SLOT(onNewConnection(int)));
}

HttpServer::~HttpServer()
{
    delete priv;
}

bool HttpServer::listen(const QHostAddress &address, quint16 port)
{
    return priv->tcpServer.listen(address, port);
}

bool HttpServer::isListening() const
{
    return priv->tcpServer.isListening();
}

quint16 HttpServer::serverPort() const
{
    return priv->tcpServer.serverPort();
}

void HttpServer::setTimeout(int msecs)
{
    priv->timeout = msecs;
}

int HttpServer::timeout() const
{
    return priv->timeout;
}

void HttpServer::close()
{
    priv->tcpServer.close();
}

void HttpServer::incomingConnection(int socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket;

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    handleConnection(socket);
}

void HttpServer::checkContinue(HttpServerRequest *request,
                               HttpServerResponse *response)
{
    response->writeContinue();
    emit requestReady(request, response);
}

void HttpServer::handleConnection(QAbstractSocket *socket)
{
    socket->setParent(this);
    HttpServerRequest *handle = new HttpServerRequest(socket, this);

    if (priv->timeout)
        handle->setTimeout(priv->timeout);

    connect(handle, SIGNAL(ready()), this, SLOT(onRequestReady()));
    connect(handle, SIGNAL(upgrade(QByteArray)),
            this, SLOT(onUpgrade(QByteArray)));
    connect(socket, SIGNAL(disconnected()), handle, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void HttpServer::upgrade(HttpServerRequest *request, const QByteArray &)
{
    request->socket()->close();
}

void HttpServer::onNewConnection(int socketDescriptor)
{
    incomingConnection(socketDescriptor);
}

void HttpServer::onRequestReady()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    Q_ASSERT(request);

    QAbstractSocket *socket = request->socket();
    HttpServerResponse *response
            = new HttpServerResponse(socket, request->responseOptions(), this);

    connect(socket, SIGNAL(disconnected()), response, SLOT(deleteLater()));
    connect(response, SIGNAL(finished()), response, SLOT(deleteLater()));

    if (request->headers().contains("Expect", "100-continue"))
        checkContinue(request, response);
    else
        emit requestReady(request, response);
}

void HttpServer::onUpgrade(const QByteArray &head)
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    Q_ASSERT(request);

    upgrade(request, head);
    delete request;
}

} // namespace Tufao
