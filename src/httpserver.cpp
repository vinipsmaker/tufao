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
#include <QtCore/QThread>
#include "headers.h"

namespace Tufao {

HttpServer::HttpServer(QObject *parent) : HttpServer(new HttpConnectionHandler(),parent)
{
}

HttpServer::HttpServer(HttpConnectionHandler *handler, QObject *parent) :
    QObject(parent),
    priv(new Priv)
{
    priv->connHandler = handler;

    //transfer ownership to us
    handler->setParent(this);

    connect(priv->connHandler,&AbstractConnectionHandler::requestReady,this,&HttpServer::requestReady);

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
    priv->connHandler->setTimeout(msecs);
}

int HttpServer::timeout() const
{
    return priv->connHandler->timeout();
}

void HttpServer::setUpgradeHandler(HttpServer::UpgradeHandler functor)
{
    if (!functor)
        return;

    priv->connHandler->setUpgradeHandler(functor);
}

HttpServer::UpgradeHandler HttpServer::defaultUpgradeHandler()
{
    return Tufao::HttpConnectionHandler::defaultUpgradeHandler();
}

AbstractConnectionHandler *HttpServer::connectionHandler() const
{
    return priv->connHandler;
}

void HttpServer::close()
{
    priv->tcpServer.close();
}

void HttpServer::onNewConnection(qintptr socketDescriptor)
{
    priv->connHandler->incomingConnection(socketDescriptor);
}

} // namespace Tufao
