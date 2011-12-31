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

#ifndef TUFAO_SERVER_H
#define TUFAO_SERVER_H

#include <QtNetwork/QTcpServer>
#include "httpserverresponse.h"

class QAbstractSocket;

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

namespace Priv {

struct HttpServer;

} // namespace Priv

/*!
  \brief The Tufao::HttpServer class provides an implementation of the HTTP
  protocol.

  To create a webserver, all you need to do is handle the
  Tufao::HttpServer::request signal.

  \code
#include <Tufao/HttpServer>

class WebServer: public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = NULL) :
        QObject(parent),
        httpServer(new Tufao::HttpServer(this))
    {
        connect(httpServer,
                SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*),
                this,
                SLOT(onRequestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));
    }

private slots:
    void onRequestReady(Tufao::HttpServerRequest *request,
                        Tufao::HttpServerResponse *response)
    {
        response->writeHead(200);
        response->setHeader("Content-Type", "text/plain");
        response->write("Hello World\n");
        response->end();
    }

private:
    Tufao::HttpServer *httpServer;
};
  \endcode

  \sa Tufao::HttpServerRequest
  Tufao::HttpServerResponse
  */
class TUFAO_EXPORT HttpServer : public QObject
{
    Q_OBJECT
public:
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
      \brief Tells the server to listen for incoming connections on address
      \p address and port \p port.

      If \p port is 0, a port is chosen automatically.

      If \p address is QHostAddress::Any, the server will listen on all
      network interfaces.
      */
    bool listen(const QHostAddress &address = QHostAddress::Any,
                quint16 port = 0);

signals:
    /*!
      This signal is emitted each time there is request. Note that there may be
      multiple requests per connection (in the case of keep-alive connections).

      You don't need to worry about delete \p request and \p response. \p
      request and \p response will be queued for deletion when the connection
      closes. Additionally, \p response will also be deleted when you are done
      with it (eg., calling Tufao::HttpServerResponse::end).

      \param request An instance of HttpServerRequest

      \param response An instance of HttpServerResponse
      */
    void requestReady(Tufao::HttpServerRequest *request,
                      Tufao::HttpServerResponse *response);

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
    virtual void incomingConnection(int socketDescriptor);

    /*!
      This virtual function is called by HttpServer when a client requests a
      http upgrade.

      The base implementation closes the connection.

      Reimplement this function to alter the server's behavior when a http
      upgrade is requested.

      \note After this function returns, \p request object is deleted.

      \note The connection object associated with \p request will be deleted
      when disconnected. If you need to delete before sooner, just call
      QIODevice::close or QObject::deleteLater.
      */
    virtual void upgrade(HttpServerRequest *request, const QByteArray &head);

private slots:
    void onNewConnection(int socketDescriptor);
    void onRequestReady(Tufao::HttpServerResponse::Options options);
    void onUpgrade(const QByteArray &head);

private:
    Priv::HttpServer *priv;
};

} // namespace Tufao

#endif // TUFAO_SERVER_H
