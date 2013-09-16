#ifndef TUFAO_ABSTRACTCONNECTIONHANDLER_H
#define TUFAO_ABSTRACTCONNECTIONHANDLER_H

#include "tufao_global.h"
#include <QObject>

class QAbstractSocket;

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

/*!
  The Tufao::AbstractConnectionHandler class provides an abstract implementation of
  a connection handler. A connection handler receives incoming socket descriptors
  and prepares it for useage inside Tufao.
  */

class TUFAO_EXPORT AbstractConnectionHandler : public QObject
{
        Q_OBJECT

    public:
        struct Priv;
        explicit AbstractConnectionHandler(QObject *parent = 0);
        virtual ~AbstractConnectionHandler();

        /*!
          This virtual function is called by HttpServer when a new connection is
          available.

          The base implementation creates a QTcpSocket, sets the socket descriptor
          and call Tufao::HttpServer::handleConnection.

          Reimplement this function to alter the server's behavior when a connection
          is available.
          */
        virtual void incomingConnection(qintptr socketDescriptor) = 0;

        /*!
          Sets the timeout of new connections to \p msecs miliseconds.

          If you set the timeout to 0, then timeout feature will be disabled. You
          should NOT disable this feature to help to protect against DoS attacks.

          The default timeout is 2 minutes (120000 miliseconds).

          \note
          You should call this function before Tufao::HttpServer::listen.
          */
        void setTimeout(int msecs = 0);

        /*!
          Returns the current set timeout.
          */
        int timeout() const;

    protected:
        /*!
          Call this function will make Tufao::HttpServer handle the connection
          \p connection.

          The Tufao::HttpServer object will take ownership of the \p connection
          object and delete it when appropriate.
          */
        virtual void handleConnection(QAbstractSocket *connection);

        /*!
          This virtual function is called by HttpServer when a client do a request
          with the HTTP header "Expect: 100-continue".

          The base implementation call Tufao::HttpServerRequest::writeContinue and
          emit the Tufao::HttpServer::requestReady signal.

          Reimplement this function to alter the server's behavior when a "Expect:
          100-continue" request is received.

          \note
          Don't delete the request or the response object, they will be deleted when
          the connection closes. If you need delete them before, just close the
          connection or call the QObject::deleteLater.

          \since
          1.0
          */
        virtual void checkContinue(HttpServerRequest &request,
                                   HttpServerResponse &response) = 0;


        virtual void handleUpgrade (HttpServerRequest &request) = 0;

    signals:
        /*!
          This signal is emitted each time there is request.

          \note
          There may be multiple requests per connection (in the case of keep-alive
          connections) and HttpServer reutilizes \p request objects, so you can't,
          as an example, create a map using \p request as key to identify sessions.

          \warning
          You MUST NOT delete \p request and \p response. \p request and \p response
          are deleted when the connection closes. Additionally, \p response will
          also be deleted when you are done with it (eg., calling
          Tufao::HttpServerResponse::end).

          \note
          If this is a POST request for a big file, you should increase the timeout
          for this individual request.

          \param request An instance of Tufao::HttpServerRequest

          \param response An instance of Tufao::HttpServerResponse

          \since
          1.0
          */
        void requestReady(Tufao::HttpServerRequest &request,
                          Tufao::HttpServerResponse &response);

    private slots:
        void onRequestReady();
        void onUpgrade();

    protected:
        explicit AbstractConnectionHandler(Priv* priv, QObject *parent = 0);
        Priv* _priv();
        const Priv *_priv() const;

    private:
        Priv* priv;
        
};

} // namespace Tufao

#endif // TUFAO_ABSTRACTCONNECTIONHANDLER_H
