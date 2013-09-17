#ifndef TUFAO_HTTPSCONNECTIONHANDLER_H
#define TUFAO_HTTPSCONNECTIONHANDLER_H

#include "httpconnectionhandler.h"

class QSslCertificate;
class QSslKey;

namespace Tufao {

/*!
  Tufao::HttpsConnectionHandler is a subclass of Tufao::HttpConnectionHandler that provides secure
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

class TUFAO_EXPORT HttpsConnectionHandler : public HttpConnectionHandler
{
    Q_OBJECT

    public:
        struct Priv;

        explicit HttpsConnectionHandler(QObject *parent = 0);

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


    // AbstractConnectionHandler interface
    public:
        virtual bool incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
};

} // namespace Tufao

#endif // TUFAO_HTTPSCONNECTIONHANDLER_H
