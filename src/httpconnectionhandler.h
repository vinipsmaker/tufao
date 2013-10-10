#ifndef TUFAO_HTTPCONNECTIONHANDLER_H
#define TUFAO_HTTPCONNECTIONHANDLER_H

#include <functional>
#include "abstractconnectionhandler.h"

namespace Tufao {

/*!
  The Tufao::HttpConnectionHandler class provides an implementation of the HTTP protocol.

  The HTTP is a stateless request-response based protoccol. It lets you create
  distributed dynamic collaborative applications.

  \sa
  Tufao::HttpServerRequest

  \sa
  Tufao::HttpServerResponse
  */

class TUFAO_EXPORT HttpConnectionHandler : public AbstractConnectionHandler
{
        Q_OBJECT

    public:
         struct Priv;
        /*!
          A typedef to http upgrade request handler.

          \sa
          AbstractHttpUpgradeHandler
          setUpgradeHandler

          \since
          1.0
         */
        typedef std::function<void(HttpServerRequest &request, const QByteArray&)>
            UpgradeHandler;

        explicit HttpConnectionHandler(QObject *parent = 0);

        /*!
          This method sets the handler that will be called to handle http upgrade
          requests.

          \note
          The connection object associated with request parameter
          (Tufao::HttpServerRequest::socket) will be deleted when disconnected. If
          you need to delete it sooner, just call QIODevice::close or
          QObject::deleteLater. \n \n

          \note
          If you pass an empty std::function object, this function does nothing.

          \sa
          defaultUpgradeHandler

          \since
          1.0
         */
        void setUpgradeHandler(UpgradeHandler functor);

        /*!
          Returns the default http upgrade request's handler.

          The default handler closes the connection.

          \since
          1.0
         */
        static UpgradeHandler defaultUpgradeHandler();
        
        // AbstractConnectionHandler interface
    public:
        virtual bool incomingConnection(qintptr socketDescriptor) override;
        virtual void closePendingConnection(qintptr socketDescriptor) override;

    protected:
        virtual void checkContinue(HttpServerRequest &request, HttpServerResponse &response) override;
        virtual void handleUpgrade(HttpServerRequest &request) override;
        HttpConnectionHandler(HttpConnectionHandler::Priv *p, QObject *parent);
};
} // namespace Tufao

#endif // TUFAO_HTTPCONNECTIONHANDLER_H
