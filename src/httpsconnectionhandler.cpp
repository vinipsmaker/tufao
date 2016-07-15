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

#include "priv/httpsconnectionhandler_p.h"
#include <QtNetwork/QSslSocket>

namespace Tufao {

HttpsConnectionHandler::HttpsConnectionHandler(QObject *parent) :
    HttpConnectionHandler(new HttpsConnectionHandler::Priv(),parent)
{

}

bool HttpsConnectionHandler::incomingConnection(qintptr socketDescriptor)
{
    Priv* p = ((Priv*)_priv());

    QSslSocket *socket = new QSslSocket;
    socket->setProtocol(QSsl::SecureProtocols);
    socket->setLocalCertificate(p->localCertificate);
    socket->setPrivateKey(p->privateKey);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return false;
    }

    socket->startServerEncryption();
    handleConnection(socket);
    return true;
}

void HttpsConnectionHandler::setLocalCertificate(const QSslCertificate &certificate)
{
    ((Priv*)_priv())->localCertificate = certificate;
}

void HttpsConnectionHandler::setPrivateKey(const QSslKey &key)
{
    ((Priv*)_priv())->privateKey = key;
}


} // namespace Tufao
