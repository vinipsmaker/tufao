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

#include "priv/httpconnectionhandler_p.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

namespace Tufao {

inline HttpConnectionHandler::Priv::Priv() :
    upgradeHandler(defaultUpgradeHandler)
{}

HttpConnectionHandler::UpgradeHandler HttpConnectionHandler::Priv::defaultUpgradeHandler{
    [](HttpServerRequest &request, const QByteArray&){
        request.socket().close();
    }
};

HttpConnectionHandler::HttpConnectionHandler(QObject *parent) :
    HttpConnectionHandler(new HttpConnectionHandler::Priv(),parent)
{
}

HttpConnectionHandler::HttpConnectionHandler(HttpConnectionHandler::Priv *p, QObject *parent)
    : AbstractConnectionHandler(p,parent)
{

}

void HttpConnectionHandler::setUpgradeHandler(HttpConnectionHandler::UpgradeHandler functor)
{
    if (!functor)
        return;

    ((Priv*)_priv())->upgradeHandler = functor;
}

bool HttpConnectionHandler::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket;

    qDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Incoming Connection");

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return false;
    }

    handleConnection(socket);
    return true;
}

void HttpConnectionHandler::checkContinue(HttpServerRequest &request, HttpServerResponse &response)
{
    response.writeContinue();
    emit requestReady(request, response);
}

void HttpConnectionHandler::handleUpgrade(HttpServerRequest &request)
{
    ((Priv*)_priv())->upgradeHandler(request,request.readBody());
}

HttpConnectionHandler::UpgradeHandler HttpConnectionHandler::defaultUpgradeHandler()
{
    return Priv::defaultUpgradeHandler;
}

} // namespace Tufao


void Tufao::HttpConnectionHandler::closePendingConnection(qintptr socketDescriptor)
{
    QTcpSocket sock;
    sock.setSocketDescriptor(socketDescriptor);

    //TODO write some status

    sock.close();
}
