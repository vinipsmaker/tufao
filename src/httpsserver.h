/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_HTTPSSERVER_H
#define TUFAO_HTTPSSERVER_H

#include "httpserver.h"

class QSslCertificate;
class QSslKey;

namespace Tufao {

/*!
  Tufao::HttpsServer is a subclass of Tufao::HttpServer that provides secure
  communication with the client.

  It does this using socket streams over TLS connections.

  This combination (HTTP + SSL/TLS) is know as HTTP Secure and provides
  encrypted communication.

  To use HTTPS in Tufão, just set the local certficate and private key before
  call Tufao::HttpsServer::listen. The default port for this protocol is 443.

  \note
  You should also pay to a trusted certificate authority to sign your
  certificate if you are willing to provide secure identification also.

  \note
  The use of HTTPS implies an extra overhead in the software, limiting the
  number of requests that can be served per time and should be moderated. It's
  common to use it only in pages that handles more sensitive information, such
  as login pages and payment transactions.

  \sa
  HttpsServer::setLocalCertificate
  HttpsServer::setPrivateKey
  */
class TUFAO_EXPORT HttpsServer : public HttpServer
{
    Q_OBJECT
public:
    explicit HttpsServer(QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpsServer();

    /*!
      Sets the local certificate to \p certificate.

      \note
      This member function should be called before Tufao::HttpsServer::listen
      */
    void setLocalCertificate(const QSslCertificate &certificate);

    /*!
      Sets the private key to \p key.

      \warning
      Remember that encryption security relies on the fact that no one knows
      your private key.

      \note
      This member function should be called before Tufao::HttpsServer::listen
      */
    void setPrivateKey(const QSslKey &key);

protected:
    void incomingConnection(int socketDescriptor);

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPSSERVER_H
