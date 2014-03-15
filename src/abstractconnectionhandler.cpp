/*  This file is part of the Tufão project
    Copyright (C) 2013 Benjamin Zeller <zeller.benjamin@web.de>
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "priv/abstractconnectionhandler_p.h"

#include "httpserverrequest.h"
#include <QtNetwork/QTcpSocket>
#include <QtCore/QThread>
#include "headers.h"

namespace Tufao {

AbstractConnectionHandler::Priv::Priv() : timeout(120000){}
AbstractConnectionHandler::Priv::~Priv(){}


AbstractConnectionHandler::AbstractConnectionHandler(AbstractConnectionHandler::Priv
                                                     *priv, QObject *parent) :
    QObject(parent),
    priv(priv)
{
}

AbstractConnectionHandler::Priv *AbstractConnectionHandler::_priv()
{
    return priv;
}

const AbstractConnectionHandler::Priv *AbstractConnectionHandler::_priv() const
{
    return priv;
}

AbstractConnectionHandler::AbstractConnectionHandler(QObject *parent) :
    AbstractConnectionHandler(new Priv(), parent)
{}

AbstractConnectionHandler::~AbstractConnectionHandler()
{
    delete priv;
}

void AbstractConnectionHandler::setTimeout(int msecs)
{
    priv->timeout = msecs;
}

int AbstractConnectionHandler::timeout() const
{
    return priv->timeout;
}

void AbstractConnectionHandler::handleConnection(QAbstractSocket *connection)
{
    HttpServerRequest *handle = new HttpServerRequest(*connection, this);
    connection->setParent(handle);

    if (priv->timeout)
        handle->setTimeout(priv->timeout);

    connect(handle, &HttpServerRequest::ready,
            this,   &AbstractConnectionHandler::onRequestReady);
    connect(handle, &HttpServerRequest::upgrade,
            this, &AbstractConnectionHandler::onUpgrade);
    connect(connection, &QAbstractSocket::disconnected,
            handle, &QObject::deleteLater);

    connect(connection, &QAbstractSocket::disconnected,
            connection, &QObject::deleteLater);

    emit newRequest(*handle);
}

void AbstractConnectionHandler::onRequestReady()
{
    HttpServerRequest *request = qobject_cast<HttpServerRequest *>(sender());
    Q_ASSERT(request);

    QAbstractSocket &socket = request->socket();
    HttpServerResponse *response
        = new HttpServerResponse(socket, request->responseOptions(), request);

    connect(&socket, &QAbstractSocket::disconnected,
            response, &QObject::deleteLater);
    connect(response, &HttpServerResponse::finished,
            response, &QObject::deleteLater);

    //in case the timout was changed in the last request handler
    //and the connection was reused
    if (priv->timeout)
        request->setTimeout(priv->timeout);

    if (request->headers().contains("Expect", "100-continue"))
        checkContinue(*request, *response);
    else
        emit requestReady(*request, *response);
}

void AbstractConnectionHandler::onUpgrade()
{
    HttpServerRequest* req = qobject_cast<HttpServerRequest*>(sender());
    if(req){
        this->handleUpgrade(*req);

        //make shure the socket is useable after the upgrade
        QAbstractSocket& socket = req->socket();
        if(socket.parent() == req){
            //socket will be automatically deleted when disconnected was emitted
            socket.setParent(0);
        }

        delete req;
    }
}

} // namespace Tufao
