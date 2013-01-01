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

#include "priv/httpsserver.h"
#include <QtNetwork/QSslSocket>

namespace Tufao {

HttpsServer::HttpsServer(QObject *parent) :
    HttpServer(parent),
    priv(new Priv)
{
}

HttpsServer::~HttpsServer()
{
    delete priv;
}

void HttpsServer::setLocalCertificate(const QSslCertificate &certificate)
{
    priv->localCertificate = certificate;
}

void HttpsServer::setPrivateKey(const QSslKey &key)
{
    priv->privateKey = key;
}

void HttpsServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *socket = new QSslSocket;
    socket->setProtocol(QSsl::TlsV1_0);
    socket->setLocalCertificate(priv->localCertificate);
    socket->setPrivateKey(priv->privateKey);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    socket->startServerEncryption();
    handleConnection(socket);
}

} // namespace Tufao
