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
#include <QtNetwork/QTcpSocket>
#include "headers.h"

namespace Tufao {

HttpServer::HttpServer(QObject *parent) :
    QObject(parent),
    priv(new Priv)
{
    connect(&priv->tcpServer, &TcpServerWrapper::newConnection,
            this, &HttpServer::onNewConnection);
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

void HttpServer::setUpgradeHandler(HttpServer::UpgradeHandler functor)
{
    if (!functor)
        return;

    priv->upgradeHandler = functor;
}

HttpServer::UpgradeHandler HttpServer::defaultUpgradeHandler()
{
    return Priv::defaultUpgradeHandler;
}

void HttpServer::close()
{
    priv->tcpServer.close();
}

void HttpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket;

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    handleConnection(socket);
}

void HttpServer::checkContinue(HttpServerRequest &request,
                               HttpServerResponse &response)
{
    response.writeContinue();
    emit requestReady(request, response);
}

void HttpServer::handleConnection(QAbstractSocket *socket)
{
    socket->setParent(this);
    HttpServerRequest *handle = new HttpServerRequest(*socket, this);

    if (priv->timeout)
        handle->setTimeout(priv->timeout);

    connect(handle, &HttpServerRequest::ready,
            this, &HttpServer::onRequestReady);
    connect(handle, &HttpServerRequest::upgrade, this, &HttpServer::onUpgrade);
    connect(socket, &QAbstractSocket::disconnected,
            handle, &QObject::deleteLater);
    connect(socket, &QAbstractSocket::disconnected,
            socket, &QObject::deleteLater);
}

void HttpServer::onNewConnection(qintptr socketDescriptor)
{
    incomingConnection(socketDescriptor);
}

void HttpServer::onRequestReady()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    Q_ASSERT(request);

    QAbstractSocket &socket = request->socket();
    HttpServerResponse *response
            = new HttpServerResponse(socket, request->responseOptions(), this);

    connect(&socket, &QAbstractSocket::disconnected,
            response, &QObject::deleteLater);
    connect(response, &HttpServerResponse::finished,
            response, &QObject::deleteLater);

    if (request->headers().contains("Expect", "100-continue"))
        checkContinue(*request, *response);
    else
        emit requestReady(*request, *response);
}

void HttpServer::onUpgrade()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    Q_ASSERT(request);

    priv->upgradeHandler(*request, request->readBody());
    delete request;
}

} // namespace Tufao
