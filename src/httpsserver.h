/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
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
    void incomingConnection(qintptr socketDescriptor) override;

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPSSERVER_H
