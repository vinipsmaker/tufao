/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#ifndef TUFAO_WEBSOCKET_H
#define TUFAO_WEBSOCKET_H

#include "abstractmessagesocket.h"
#include "headers.h"

#include <QAbstractSocket>

namespace Tufao {

class HttpServerRequest;

/*!
  This class represents a WebSocket connection.

  WebSocket is a protocol designed to allow HTTP user agents and servers
  communicates using a two-way protocol. It's possible to upgrade a established
  HTTP connection to a WebSocket connection.

  \sa
  Tufao::AbstractMessageSocket

  \since 0.2
  */
class TUFAO_EXPORT WebSocket : public AbstractMessageSocket
{
    Q_OBJECT
public:
    /*!
      This enum describes the possible erros tha can occur.
      */
    enum Error
    {
        /*!
          No error
          */
        NO_ERROR = 0,
        /*!
          See QAbstractSocket::ConnectionRefusedError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        CONNECTION_REFUSED,
        /*!
          See QAbstractSocket::RemoteHostClosedError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        REMOTE_HOST_CLOSED,
        /*!
          See QAbstractSocket::HostNotFoundError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        HOST_NOT_FOUND,
        /*!
          See QAbstractSocket::SocketAccessError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        ACCESS_ERROR,
        /*!
          See QAbstractSocket::SocketResourceError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        OUT_OF_RESOURCES,
        /*!
          See QAbstractSocket::SocketTimeoutError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        SOCKET_TIMEOUT,
        /*!
          See QAbstractSocket::NetworkError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        NETWORK_ERROR,
        /*!
          See QAbstractSocket::UnsupportedSocketOperationError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        UNSUPPORTED_SOCKET_OPERATION,
        /*!
          See QAbstractSocket::ProxyAuthenticationRequiredError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        PROXY_AUTHENTICATION_REQUIRED,
        /*!
          See QAbstractSocket::SslHandshakeFailedError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        SSL_HANDSHAKE_FAILED,
        /*!
          See QAbstractSocket::ProxyConnectionRefusedError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        PROXY_CONNECTION_REFUSED,
        /*!
          See QAbstractSocket::ProxyConnectionClosedError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        PROXY_CONNECTION_CLOSED,
        /*!
          See QAbstractSocket::ProxyConnectionTimeoutError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        PROXY_CONNECTION_TIMEOUT,
        /*!
          See QAbstractSocket::ProxyNotFoundError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        PROXY_NOT_FOUND,
        /*!
          See QAbstractSocket::ProxyProtocolError.

          It can happen during the opening handshake (only when acting as
          client).
          */
        PROXY_PROTOCOL_ERROR,
        /*!
          It occurs when the server doesn't support WebSocket for the resource
          asked for (or for any resource at all).

          It can happen during the opening handshake (only when acting as
          client).
          */
        WEBSOCKET_HANDSHAKE_FAILED,
        /*!
          It occurs when the remote peer (or an intermediary) violates the
          WebSocket protocol.
          */
        WEBSOCKET_PROTOCOL_ERROR,
        /*!
          Uknown error.

          You found the chaos.
          */
        UNKNOWN_ERROR
    };

    /*!
      This enum represents the possible message's types that WebSocket supports.
      */
    enum MessageType
    {
        /*!
          UTF8 messages.
          */
        TEXT_MESSAGE,
        /*!
          Binary messages.
          */
        BINARY_MESSAGE
    };

    /*!
      Constructs a Tufao::WebSocket object.

      \p parent is passed to the QObject constructor.
      */
    explicit WebSocket(QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~WebSocket();

    /*!
      Execute the steps to establish a WebSocket connection with the server
      represented by \p address and \p port on the given \p resource.

      You can send extra headers in the WebSocket opening handshake request
      using the \p headers argument.

      \note
      Optional headers such as Origin aren't added by default. If you want to
      use one of the headers, you should add them explicitly. The list of common
      optional headers is the following:
        - Origin: RFC 6454. If your software is a browser client, this field is
          mandatory, not optional.
        - Sec-WebSocket-Protocol: If present, this value indicates one or more
          comma-separated subprotocol the client wishes to speak, ordered by
          preference.

      \note
      If the object fail to establish a connection, it will emit disconnected
      signal. If it procceds, it will emit the connected signal.
      */
    bool connectToHost(const QHostAddress &address, quint16 port,
                       const QByteArray &resource,
                       const Headers &headers = Headers());

    /*!
      This is an overloaded function.

      It uses port 80 to establish the connection.
      */
    bool connectToHost(const QHostAddress &address, const QByteArray &resource,
                       const Headers &headers = Headers());

    /*!
      The same as WebSocket::connectToHost, but uses a TLS connection.

      \sa
      Tufao::WebSocket::connectToHost
      */
    bool connectToHostEncrypted(const QString &address, quint16 port,
                                const QByteArray &resource,
                                const Headers &headers = Headers());

    /*!
      This is an overloaded function.

      It uses port 443 to establish the connection.
      */
    bool connectToHostEncrypted(const QString &address,
                                const QByteArray &resource,
                                const Headers &headers = Headers());

    /*!
      It establish a WebSocket connection initiated by \p request with \p head
      data.

      \note
      You should call this function only after \p request emitts the
      Tufao::HttpServerRequest::upgrade signal.

      \note
      Tufao::WebSocket won't treat optional headers found in \p request. If you
      want to respond to these headers in the opening handshake response, just
      put the headers you want to send in the response in the \p headers
      argument. Some headers you may want to treat are:
        - Origin: Compare the Origin to the Host to block malicious scripts
          coming from web browsers.
        - Cookie
        - Sec-WebSocket-Protocol: The subprotocol the client wishes to speak.

      \note
      If you want to perform additional client authentication, you should
      start the handshake only after the authentication occurs. You can use the
      401 status code with "WWW-Authenticate" header to perform the
      authentication, among other methods.

      \note
      You can use a 3xx status code to redirect the client.

      \note
      If the handshake fail, the method will write the appropriate message to
      the socket and return false.
      */
    bool startServerHandshake(const HttpServerRequest *request,
                              const QByteArray &head,
                              const Headers &headers = Headers());

    /*!
      Set the type of messages sent through WebSocket::sendMessage method.

      \note
      Another way of choose the type of sent messages is through
      WebSocket::sendBinaryMessage and WebSocket::sendUtf8Message methods.

      \note
      The default value is BINARY_MESSAGE
      */
    void setMessagesType(MessageType type);

    /*!
      Return the current type of messages what will be sent through
      WebSocket::sendMessage method.

      \sa
      Tufao::WebSocket::setMessagesType
      */
    MessageType messagesType() const;

    /*!
      Returns the type of last error that occurred.
      */
    Error error() const;

    /*!
      Returns a human-readable description of the last error that occurred.
      */
    QString errorString() const;

    /*!
      It reads the supported sub-protocols from the appropriate fields and
      return them.
      */
    static QList<QByteArray> supportedProtocols(const Headers &headers);

signals:
    /*!
      This signal is emitted when a pong frame is received.

      Pong frames are sent in response to ping frames.

      \sa
      WebSocket::ping
      */
    void pong(QByteArray data);

public slots:
    void close();
    bool sendMessage(const QByteArray &msg);

    /*!
      Sends a binary message over the connection.

      \sa
      WebSocket::sendMessage
      WebSocket::messagesType
      */
    bool sendBinaryMessage(const QByteArray &msg);

    /*!
      Sends a UTF-8 text message over the connection.

      \sa
      WebSocket::sendMessage
      WebSocket::messagesType
      */
    bool sendUtf8Message(const QByteArray &msg);

    /*!
      Sends a ping frame over the connection.

      A WebSocket peer that receives a ping frame must respond with a pong frame
      as soon as pratical.

      You can use a ping frame to measure the connection lag, to test the
      connectivity availability, among other.
      */
    bool ping(const QByteArray &data);

private slots:
    void onSocketError(QAbstractSocket::SocketError error);
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    void connectToHost(QAbstractSocket *socket, const QByteArray &resource,
                       const Headers &headers = Headers());

    bool isResponseOkay();
    void onClientHandshakeError();

    void close(quint16 code);

    void readData(const QByteArray &data);
    void parseBuffer();
    bool parseFrame();
    bool parseSize16();
    bool parseSize64();
    bool parseMaskingKey();
    bool parsePayloadData();

    void decodeFragment(QByteArray &fragment);
    void evaluateControlFrame();

    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_WEBSOCKET_H
