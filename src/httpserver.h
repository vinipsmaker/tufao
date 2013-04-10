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

#ifndef TUFAO_SERVER_H
#define TUFAO_SERVER_H

#include <functional>
#include <QtNetwork/QTcpServer>
#include "tufao_global.h"

class QAbstractSocket;

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

/*!
  The Tufao::HttpServer class provides an implementation of the HTTP protocol.

  The HTTP is a stateless request-response based protoccol. It let you create
  distributed dynamic collaborative applications.

  To create a webserver, all you need to do is call Tufao::HttpServer::listen
  and handle the Tufao::HttpServer::requestReady signal.

  \include simpleserver.cpp

  \sa
  Tufao::HttpServerRequest

  \sa
  Tufao::HttpServerResponse
  */
class TUFAO_EXPORT HttpServer : public QObject
{
    Q_OBJECT
public:
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

    /*!
      Constructs a Tufao::HttpServer object.

      \p parent is passed to the QObject constructor.
      */
    explicit HttpServer(QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpServer();

    /*!
      Tells the server to listen for incoming connections on address \p address
      and port \p port.

      If \p port is 0, a port is chosen automatically. The default registered
      port to HTTP server is 80.

      If \p address is QHostAddress::Any, the server will listen on all network
      interfaces.

      \return true on success

      \sa
      Tufao::HttpServer::isListening
      Tufao::HttpServer::serverPort
      */
    bool listen(const QHostAddress &address = QHostAddress::Any,
                quint16 port = 0);

    /*!
      Returns true if the server is listening for incoming connections.
      */
    bool isListening() const;

    /*!
      Returns the server's port if the server is listening; otherwise returns 0.

      \sa
      Tufao::HttpServer::listen
      Tufao::HttpServer::isListening
      */
    quint16 serverPort() const;

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

public slots:
    /*!
      Closes the server. The server will no longer listen for incoming
      connections.
      */
    void close();

protected:
    /*!
      Call this function will make Tufao::HttpServer handle the connection
      \p connection.

      The Tufao::HttpServer object will take ownership of the \p connection
      object and delete it when appropriate.
      */
    void handleConnection(QAbstractSocket *connection);

    /*!
      This virtual function is called by HttpServer when a new connection is
      available.

      The base implementation creates a QTcpSocket, sets the socket descriptor
      and call Tufao::HttpServer::handleConnection.

      Reimplement this function to alter the server's behavior when a connection
      is available.
      */
    virtual void incomingConnection(qintptr socketDescriptor);

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
                               HttpServerResponse &response);

private slots:
    void onNewConnection(qintptr socketDescriptor);
    void onRequestReady();
    void onUpgrade();

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_SERVER_H
