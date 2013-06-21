/*  This file is part of the Tufão project
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

#include "priv/websocket.h"
#include "httpserverrequest.h"
#include "headers.h"

#include <QtCore/QCryptographicHash>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QSslSocket>

#if defined(NO_ERROR) && defined(_WIN32)
# undef NO_ERROR
#endif

// Writes a string without the '\0' char using function \p func
#define WRITE_STRING(func, chunk) (func)(chunk, sizeof(chunk) - 1)

static const char crlf[] = "\r\n";
#define CRLF crlf, sizeof(crlf) - 1

namespace Tufao {

WebSocket::WebSocket(QObject *parent) :
    AbstractMessageSocket(parent),
    priv(new Priv)
{
}

WebSocket::~WebSocket()
{
    delete priv;
}

bool WebSocket::connectToHost(const QHostAddress &address, quint16 port,
                              const QByteArray &resource,
                              const Headers &headers)
{
    if (priv->state != Priv::CLOSED)
        return false;

    QAbstractSocket *socket = new QTcpSocket(this);

    if (!headers.contains("Host")) {
        Headers newHeaders(headers);
        newHeaders.insert("Host", (address.toString() + ':'
                                   + QString::number(port)).toUtf8());
        connectToHost(socket, resource, newHeaders);
    } else {
        connectToHost(socket, resource, headers);
    }

    socket->connectToHost(address, port);

    return true;
}

bool WebSocket::connectToHost(const QHostAddress &address,
                              const QByteArray &resource,
                              const Headers &headers)
{
    return connectToHost(address, 80, resource, headers);
}

bool WebSocket::connectToHost(const QString &hostname, quint16 port,
                              const QByteArray &resource,
                              const Headers &headers)
{
    if (priv->state != Priv::CLOSED)
        return false;

    QAbstractSocket *socket = new QTcpSocket(this);

    if (!headers.contains("Host")) {
        Headers newHeaders(headers);
        newHeaders.insert("Host", hostname.toUtf8());
        connectToHost(socket, resource, newHeaders);
    } else {
        connectToHost(socket, resource, headers);
    }

    socket->connectToHost(hostname, port);

    return true;
}

bool WebSocket::connectToHost(const QString &hostname,
                              const QByteArray &resource,
                              const Headers &headers)
{
    return connectToHost(hostname, 80, resource, headers);
}

bool WebSocket::connectToHostEncrypted(const QString &hostname, quint16 port,
                                       const QByteArray &resource,
                                       const Headers &headers,
                                       const QList<QSslError> &ignoredSslErrors)
{
    if  (priv->state != Priv::CLOSED)
        return false;

    QSslSocket *socket = new QSslSocket(this);

    socket->ignoreSslErrors(ignoredSslErrors);

    if (!headers.contains("Host")) {
        Headers newHeaders(headers);
        newHeaders.insert("Host", hostname.toUtf8());
        connectToHost(socket, resource, newHeaders);
    } else {
        connectToHost(socket, resource, headers);
    }

    socket->connectToHostEncrypted(hostname, port);

    return true;
}

bool WebSocket::connectToHostEncrypted(const QString &hostname, quint16 port,
                                       const QByteArray &resource,
                                       const Headers &headers)
{
    return connectToHostEncrypted(hostname, port, resource, headers,
                                  QList<QSslError>());
}

bool WebSocket::connectToHostEncrypted(const QString &hostname,
                                       const QByteArray &resource,
                                       const Headers &headers)
{
    return connectToHostEncrypted(hostname, 443, resource, headers);
}

bool WebSocket::connectToHostEncrypted(const QHostAddress &address,
                                       quint16 port, const QByteArray &resource,
                                       const Headers &headers)
{
    if (!headers.contains("Host")) {
        Headers newHeaders(headers);
        newHeaders.insert("Host", (address.toString() + ':'
                                   + QString::number(port)).toUtf8());
        return connectToHostEncrypted(address.toString(), port, resource,
                                      newHeaders);
    }

    return connectToHostEncrypted(address.toString(), port, resource, headers);
}

bool WebSocket::connectToHostEncrypted(const QHostAddress &address,
                                       const QByteArray &resource,
                                       const Headers &headers)
{
    return connectToHostEncrypted(address, 443, resource, headers);
}

bool WebSocket::startServerHandshake(const HttpServerRequest &request,
                                     const QByteArray &head,
                                     const Headers &extraHeaders)
{
    QAbstractSocket *socket = &request.socket();
    Headers headers = request.headers();
    if (!hasValueCaseInsensitively(headers.values("Upgrade"), "websocket")) {
        WRITE_STRING(socket->write,
                     "HTTP/1.1 400 Bad Request\r\n"
                     "Connection: keep-alive\r\n"
                     "Content-Length: 0\r\n"
                     "\r\n");
        return false;
    }

    if (!headers.contains("Sec-WebSocket-Version", "13")) {
        WRITE_STRING(socket->write,
                     "HTTP/1.1 426 Upgrade Required\r\n"
                     "Connection: keep-alive\r\n"
                     "Content-Length: 0\r\n"
                     "Sec-WebSocket-Version: 13\r\n"
                     "\r\n");
        return false;
    }

    QByteArray key = headers.value("Sec-WebSocket-Key");
    if (key.size() != 24)
        return false;

    priv->socket = socket;
    priv->isClientNode = false;
    priv->state = Priv::OPEN;
    priv->lastError = WebSocketError::NO_ERROR;

    WRITE_STRING(socket->write,
                 "HTTP/1.1 101 Switching Protocols\r\n"
                 "Connection: Upgrade\r\n"
                 "Upgrade: websocket\r\n");

    for (Headers::const_iterator i = extraHeaders.begin()
         ;i != extraHeaders.end();++i) {
        socket->write(i.key());
        WRITE_STRING(socket->write, ": ");
        socket->write(i.value());
        socket->write(CRLF);
    }

    WRITE_STRING(socket->write, "Sec-WebSocket-Accept: ");
    WRITE_STRING(key.append, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    socket->write(QCryptographicHash::hash(key, QCryptographicHash::Sha1)
                  .toBase64());

    WRITE_STRING(socket->write, "\r\n\r\n");

    connect(socket, &QAbstractSocket::readyRead, this, &WebSocket::onReadyRead);
    connect(socket, &QAbstractSocket::disconnected,
            this, &WebSocket::onDisconnected);

    emit connected();

    if (head.size())
        readData(head);

    return true;
}

bool WebSocket::startServerHandshake(HttpServerRequest &request,
                                     const Headers &extraHeaders)
{
    return startServerHandshake(request, request.readBody(), extraHeaders);
}

void WebSocket::setMessagesType(WebSocketMessageType type)
{
    priv->messageType = type;
}

WebSocketMessageType WebSocket::messagesType() const
{
    return priv->messageType;
}

WebSocketError WebSocket::error() const
{
    return priv->lastError;
}

QString WebSocket::errorString() const
{
    switch (priv->lastError) {
    case WebSocketError::NO_ERROR:
        return QString();
    case WebSocketError::CONNECTION_REFUSED:
        return "See QAbstractSocket::ConnectionRefusedError";
    case WebSocketError::REMOTE_HOST_CLOSED:
        return "See QAbstractSocket::RemoteHostClosedError";
    case WebSocketError::HOST_NOT_FOUND:
        return "See QAbstractSocket::HostNotFoundError";
    case WebSocketError::ACCESS_ERROR:
        return "See QAbstractSocket::SocketAccessError";
    case WebSocketError::OUT_OF_RESOURCES:
        return "See QAbstractSocket::SocketResourceError";
    case WebSocketError::SOCKET_TIMEOUT:
        return "See QAbstractSocket::SocketTimeoutError";
    case WebSocketError::NETWORK_ERROR:
        return "See QAbstractSocket::NetworkError";
    case WebSocketError::UNSUPPORTED_SOCKET_OPERATION:
        return "See QAbstractSocket::UnsupportedSocketOperationError";
    case WebSocketError::PROXY_AUTHENTICATION_REQUIRED:
        return "See QAbstractSocket::ProxyAuthenticationRequiredError";
    case WebSocketError::SSL_HANDSHAKE_FAILED:
        return "See QAbstractSocket::SslHandshakeFailedError";
    case WebSocketError::PROXY_CONNECTION_REFUSED:
        return "See QAbstractSocket::ProxyConnectionRefusedError";
    case WebSocketError::PROXY_CONNECTION_CLOSED:
        return "See QAbstractSocket::ProxyConnectionClosedError";
    case WebSocketError::PROXY_CONNECTION_TIMEOUT:
        return "See QAbstractSocket::ProxyConnectionTimeoutError";
    case WebSocketError::PROXY_NOT_FOUND:
        return "See QAbstractSocket::ProxyNotFoundError";
    case WebSocketError::PROXY_PROTOCOL_ERROR:
        return "See QAbstractSocket::ProxyProtocolError";
    case WebSocketError::WEBSOCKET_HANDSHAKE_FAILED:
        return "It failed to establish a WebSocket connection";
    case WebSocketError::WEBSOCKET_PROTOCOL_ERROR:
        return "An invalid WebSocket frame was received";
    default:
        return "Unknown error";
    }
}

QHostAddress WebSocket::peerAddress() const
{
    if (!priv->socket)
        return QHostAddress::Null;

    return priv->socket->peerAddress();
}

quint16 WebSocket::peerPort() const
{
    if (!priv->socket)
        return 0;

    return priv->socket->peerPort();
}

QList<QByteArray> WebSocket::supportedProtocols(const Headers &headers)
{
    QList<QByteArray> values;

    QList<QByteArray> fields = headers.values("Sec-WebSocket-Protocol");
    for (QList<QByteArray>::const_iterator i = fields.constBegin()
         ;i != fields.constEnd();++i) {
        QList<QByteArray> header = i->split(',');
        for (QList<QByteArray>::const_iterator i = header.constBegin()
             ;i != header.constEnd();++i) {
            values.push_back(i->trimmed());
        }
    }

    return values;
}

void WebSocket::close()
{
    close(StatusCode::NORMAL);
}

bool WebSocket::sendMessage(const QByteArray &msg)
{
    switch (priv->messageType) {
    case WebSocketMessageType::BINARY_MESSAGE:
        return sendBinaryMessage(msg);
    case WebSocketMessageType::TEXT_MESSAGE:
        return sendUtf8Message(msg);
    default:
        qWarning("Tufao::WebSocket::sendMessage: Invalid message type to send");
        return false;
    }
}

bool WebSocket::sendBinaryMessage(const QByteArray &msg)
{
    if (priv->state != Priv::OPEN)
        return false;

    Priv::Frame frame = Priv::Frame::standardFrame(priv->isClientNode);
    frame.setFin();
    frame.setOpcode(FrameType::BINARY);

    frame.writePayload(priv->socket, priv->isClientNode, msg);

    return true;
}

bool WebSocket::sendUtf8Message(const QByteArray &msg)
{
    if (priv->state != Priv::OPEN)
        return false;

    Priv::Frame frame = Priv::Frame::standardFrame(priv->isClientNode);
    frame.setFin();
    frame.setOpcode(FrameType::TEXT);

    frame.writePayload(priv->socket, priv->isClientNode, msg);

    return true;
}

bool WebSocket::ping(const QByteArray &data)
{
    if (priv->state != Priv::OPEN)
        return false;

    Priv::Frame frame = Priv::Frame::controlFrame(priv->isClientNode);
    frame.setOpcode(FrameType::PING);
    frame.writePayload(priv->socket, priv->isClientNode, data);

    return true;
}

void WebSocket::onSocketError(QAbstractSocket::SocketError error)
{
    priv->socket->deleteLater();
    priv->socket = NULL;
    delete priv->clientNode;
    priv->clientNode = NULL;
    priv->state = Priv::CLOSED;

    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        priv->lastError = WebSocketError::CONNECTION_REFUSED;
        break;
    case QAbstractSocket::RemoteHostClosedError:
        priv->lastError = WebSocketError::REMOTE_HOST_CLOSED;
        break;
    case QAbstractSocket::HostNotFoundError:
        priv->lastError = WebSocketError::HOST_NOT_FOUND;
        break;
    case QAbstractSocket::SocketAccessError:
        priv->lastError = WebSocketError::ACCESS_ERROR;
        break;
    case QAbstractSocket::SocketTimeoutError:
        priv->lastError = WebSocketError::SOCKET_TIMEOUT;
        break;
    case QAbstractSocket::SocketResourceError:
        priv->lastError = WebSocketError::OUT_OF_RESOURCES;
        break;
    case QAbstractSocket::NetworkError:
        priv->lastError = WebSocketError::NETWORK_ERROR;
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        priv->lastError = WebSocketError::UNSUPPORTED_SOCKET_OPERATION;
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        priv->lastError = WebSocketError::PROXY_AUTHENTICATION_REQUIRED;
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        priv->lastError = WebSocketError::SSL_HANDSHAKE_FAILED;
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        priv->lastError = WebSocketError::PROXY_CONNECTION_REFUSED;
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        priv->lastError = WebSocketError::PROXY_CONNECTION_CLOSED;
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        priv->lastError = WebSocketError::PROXY_CONNECTION_TIMEOUT;
        break;
    case QAbstractSocket::ProxyNotFoundError:
        priv->lastError = WebSocketError::PROXY_NOT_FOUND;
        break;
    case QAbstractSocket::ProxyProtocolError:
        priv->lastError = WebSocketError::PROXY_PROTOCOL_ERROR;
        break;
    default:
        priv->lastError = WebSocketError::UNKNOWN_ERROR;
    }

    emit disconnected();
}

void WebSocket::onSslErrors(const QList<QSslError> &)
{
    priv->socket->deleteLater();
    priv->socket = NULL;
    delete priv->clientNode;
    priv->clientNode = NULL;
    priv->state = Priv::CLOSED;
    priv->lastError = WebSocketError::SSL_HANDSHAKE_FAILED;

    emit disconnected();
}

void WebSocket::onConnected()
{
    WRITE_STRING(priv->socket->write, "GET ");
    priv->socket->write(priv->clientNode->resource);
    WRITE_STRING(priv->socket->write, " HTTP/1.1\r\n");

    for (Headers::const_iterator i = priv->clientNode->headers.constBegin()
         ;i != priv->clientNode->headers.constEnd();++i) {
        priv->socket->write(i.key());
        priv->socket->write(": ", 2);
        priv->socket->write(i.value());
        priv->socket->write(CRLF);
    }
    WRITE_STRING(priv->socket->write,
                 "Upgrade: websocket\r\n"
                 "Connection: Upgrade\r\n"
                 "Sec-WebSocket-Version: 13\r\n"

                 "Sec-WebSocket-Key: ");
    {
        static const int SUBSTR_SIZE = int(sizeof(int));
        union
        {
            int i;
            char str[sizeof(int)];
        } chunk;
        QByteArray headerValue;
        headerValue.reserve(16);

        for (int i = 0;i < 16;i += SUBSTR_SIZE) {
            chunk.i = qrand();
            headerValue.append(chunk.str, qMin(SUBSTR_SIZE, 16 - i));
        }

        headerValue = headerValue.toBase64();

        priv->clientNode->expectedWebSocketAccept
                = QCryptographicHash::hash(headerValue + "258EAFA5-E914-47DA"
                                           "-95CA-C5AB0DC85B11",
                                           QCryptographicHash::Sha1).toBase64();

        priv->socket->write(headerValue);
    }
    WRITE_STRING(priv->socket->write, "\r\n\r\n");

    if (qobject_cast<QSslSocket*>(priv->socket)) {
        disconnect(priv->socket, SIGNAL(encrypted()), this, SLOT(onConnected()));
        disconnect(priv->socket, SIGNAL(error(QAbstractSocket::SocketError)),
                   this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    } else {
        disconnect(priv->socket, SIGNAL(connected()), this, SLOT(onConnected()));
    }

    connect(priv->socket, &QAbstractSocket::disconnected,
            this, &WebSocket::onDisconnected);

    priv->clientNode->headers.clear();
    priv->clientNode->resource.clear();
}

void WebSocket::onReadyRead()
{
    readData(priv->socket->readAll());
}

void WebSocket::onDisconnected()
{
    priv->socket->deleteLater();
    priv->socket = NULL;
    priv->state = Priv::CLOSED;
    emit disconnected();
}

void WebSocket::connectToHost(QAbstractSocket *socket,
                              const QByteArray &resource,
                              const Headers &headers)
{
    priv->isClientNode = true;
    priv->state = Priv::CONNECTING;
    priv->lastError = WebSocketError::NO_ERROR;
    priv->socket = socket;

    if (!priv->clientNode)
        priv->clientNode = new WebSocketClientNode;

    priv->clientNode->headers = headers;
    priv->clientNode->resource = resource;

    if (qobject_cast<QSslSocket*>(priv->socket)) {
        connect(priv->socket, SIGNAL(encrypted()), this, SLOT(onConnected()));
        connect(priv->socket, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(onSslErrors(QList<QSslError>)));
    } else {
        connect(priv->socket, SIGNAL(connected()), this, SLOT(onConnected()));
    }

    connect(priv->socket, &QAbstractSocket::readyRead,
            this, &WebSocket::onReadyRead);
    {
        void(QAbstractSocket::*errorSignal)(QAbstractSocket::SocketError)
                = &QAbstractSocket::error;
        connect(priv->socket, errorSignal, this, &WebSocket::onSocketError);
    }
}

inline bool WebSocket::isResponseOkay()
{
    if (!hasValueCaseInsensitively(priv->clientNode->response.headers
                                   .values("Upgrade"), "websocket"))
        return false;

    if (!priv->clientNode->response.headers
            .contains("Sec-WebSocket-Accept",
                      priv->clientNode->expectedWebSocketAccept))
        return false;

    if (priv->clientNode->response.headers.contains("Sec-WebSocket-Extensions"))
        return false;

    {
        QList<QByteArray> protocol = priv->clientNode->response.headers
                .values("Sec-WebSocket-Protocol");

        if (protocol.size() == 0)
            return true;

        if (protocol.size() > 1)
            return false;

        QList<QByteArray> supported
                = supportedProtocols(priv->clientNode->headers);
        if (!supported.contains(protocol[0]))
            return false;
    }

    return true;
}

inline void WebSocket::onClientHandshakeError()
{
    priv->socket->deleteLater();
    priv->socket = NULL;
    delete priv->clientNode;
    priv->clientNode = NULL;
    priv->state = Priv::CLOSED;
    priv->lastError = WebSocketError::WEBSOCKET_HANDSHAKE_FAILED;
    emit disconnected();
}

inline void WebSocket::close(quint16 code)
{
    if (priv->state == Priv::CLOSING
            || priv->state == Priv::CLOSED)
        return;

    uchar chunk[2];
    qToBigEndian(code, chunk);

    QByteArray data(reinterpret_cast<char*>(chunk), 2);

    Priv::Frame frame = Priv::Frame::controlFrame(priv->isClientNode);
    frame.setOpcode(FrameType::CONNECTION_CLOSE);

    frame.writePayload(priv->socket, priv->isClientNode, data);

    priv->state = Priv::CLOSING;
}

inline void WebSocket::readData(const QByteArray &data)
{
    priv->buffer += data;
    switch (priv->state) {
    case Priv::CONNECTING:
        if (priv->clientNode->response.execute(priv->buffer)) {
            if (!isResponseOkay()) {
                onClientHandshakeError();
                break;
            }
        } else {
            if (priv->clientNode->response.error()
                    || priv->clientNode->response.ready) {
                onClientHandshakeError();
            }

            break;
        }

        delete priv->clientNode;
        priv->clientNode = NULL;
        priv->state = Priv::OPEN;
        emit connected();
    case Priv::OPEN:
    case Priv::CLOSING:
        parseBuffer();
        break;
    case Priv::CLOSED:
    default:
        qWarning("WebSocket: data received while in closed state");
    }
}

inline void WebSocket::parseBuffer()
{
    while (true) {
        switch (priv->parsingState) {
        case Priv::PARSING_FRAME:
            if (!parseFrame()) return;
            break;
        case Priv::PARSING_SIZE_16BIT:
            if (!parseSize16()) return;
            break;
        case Priv::PARSING_SIZE_64BIT:
            if (!parseSize64()) return;
            break;
        case Priv::PARSING_MASKING_KEY:
            if (!parseMaskingKey()) return;
            break;
        case Priv::PARSING_PAYLOAD_DATA:
            if (!parsePayloadData()) return;
        }
    }
}

inline bool WebSocket::parseFrame()
{
    if (priv->buffer.size() < int(sizeof(Priv::Frame)))
        return false;

    for (uint i = 0;i != 2;++i)
        priv->frame.bytes[i] = priv->buffer[i];

    priv->buffer.remove(0, 2);

    if (!priv->frame.fin() && priv->frame.isControlFrame()) {
        close(StatusCode::PROTOCOL_ERROR);
        priv->lastError = WebSocketError::WEBSOCKET_PROTOCOL_ERROR;
        priv->socket->close();
        return false;
    }

    if ((priv->frame.masked() && priv->isClientNode)
            || (!priv->frame.masked() && !priv->isClientNode)) {
        close(StatusCode::PROTOCOL_ERROR);
        priv->lastError = WebSocketError::WEBSOCKET_PROTOCOL_ERROR;
        priv->socket->close();
        return false;
    }

    if (priv->frame.payloadLength() == 126) {
        priv->parsingState = Priv::PARSING_SIZE_16BIT;
    } else if (priv->frame.payloadLength() == 127) {
        priv->parsingState = Priv::PARSING_SIZE_64BIT;
    } else {
        priv->remainingPayloadSize = priv->frame.payloadLength();

        if (!priv->isClientNode)
            priv->parsingState = Priv::PARSING_MASKING_KEY;
        else
            priv->parsingState = Priv::PARSING_PAYLOAD_DATA;
    }

    return true;
}

inline bool WebSocket::parseSize16()
{
    if (priv->buffer.size() < int(sizeof(quint16)))
        return false;

    uchar size[2];

    for (uint i = 0;i != uint(sizeof(quint16));++i)
        size[i] = priv->buffer[i];

    priv->buffer.remove(0, sizeof(quint16));

    priv->remainingPayloadSize = qFromBigEndian<quint16>(size);

    if (!priv->isClientNode)
        priv->parsingState = Priv::PARSING_MASKING_KEY;
    else
        priv->parsingState = Priv::PARSING_PAYLOAD_DATA;

    return true;
}

inline bool WebSocket::parseSize64()
{
    if (priv->buffer.size() < int(sizeof(quint64)))
        return false;

    uchar size[8];

    for (uint i = 0;i != uint(sizeof(quint64));++i)
        size[i] = priv->buffer[i];

    priv->buffer.remove(0, sizeof(quint64));

    priv->remainingPayloadSize = qFromBigEndian<quint64>(size);

    if (!priv->isClientNode)
        priv->parsingState = Priv::PARSING_MASKING_KEY;
    else
        priv->parsingState = Priv::PARSING_PAYLOAD_DATA;

    return true;
}

inline bool WebSocket::parseMaskingKey()
{
    if (priv->buffer.size() < int(sizeof(quint32)))
        return false;

    priv->maskingIndex = 0;

    for (int i = 0;i != 4;++i)
        *(reinterpret_cast<char *>(priv->maskingKey + i)) = priv->buffer[i];

    priv->buffer.remove(0, sizeof(quint32));

    priv->parsingState = Priv::PARSING_PAYLOAD_DATA;

    return true;
}

inline bool WebSocket::parsePayloadData()
{
    // This only happens in zero-length payload data
    if (!priv->remainingPayloadSize) {
        if (priv->frame.isControlFrame())
            evaluateControlFrame();

        priv->parsingState = Priv::PARSING_FRAME;

        return true;
    }

    if (!priv->buffer.size())
        return false;

    {
        QByteArray chunk = priv->buffer.left(priv->remainingPayloadSize);
        priv->buffer.remove(0, chunk.size());
        priv->remainingPayloadSize -= chunk.size();
        decodeFragment(chunk);
        priv->payload += chunk;
    }

    if (priv->remainingPayloadSize)
        return false;

    priv->parsingState = Priv::PARSING_FRAME;

    if (priv->frame.fin()) {
        // FINAL
        if (priv->frame.isControlFrame()) {
            evaluateControlFrame();
        } else {
            if (priv->frame.opcode() == FrameType::CONTINUATION) {
                // CONTINUATION
                QByteArray chunk(priv->fragment);
                priv->fragment.clear();
                emit newMessage(chunk);
            } else {
                // NON-CONTINUATION
                QByteArray chunk(priv->payload);
                priv->payload.clear();
                emit newMessage(chunk);
            }
        }
    } else {
        // NON-FINAL
        priv->fragment += priv->payload;
        priv->payload.clear();

        if (priv->frame.opcode() != FrameType::CONTINUATION)
            priv->fragmentOpcode = priv->frame.opcode();
    }

    return true;
}

inline void WebSocket::decodeFragment(QByteArray &fragment)
{
    if (priv->isClientNode)
        return;

    for (int i = 0;i != fragment.size();++i) {
        fragment[i] = fragment[i] ^ priv->maskingKey[priv->maskingIndex % 4];
        priv->maskingIndex += 1;
    }
}

inline void WebSocket::evaluateControlFrame()
{
    switch (priv->frame.opcode()) {
    case FrameType::CONNECTION_CLOSE:
    {
        if (priv->state == Priv::CLOSING) {
            priv->socket->close();
        } else {
            Priv::Frame frame = Priv::Frame::controlFrame(priv->isClientNode);
            frame.setOpcode(FrameType::CONNECTION_CLOSE);
            frame.writePayload(priv->socket, priv->isClientNode, priv->payload);

            priv->state = Priv::CLOSING;
            if (!priv->isClientNode)
                priv->socket->close();
        }
        break;
    }
    case FrameType::PING:
    {
        Priv::Frame frame = Priv::Frame::controlFrame(priv->isClientNode);
        frame.setOpcode(FrameType::PONG);
        frame.writePayload(priv->socket, priv->isClientNode, priv->payload);
        break;
    }
    case FrameType::PONG:
        emit pong(priv->payload);
        break;
    default:
        priv->lastError = WebSocketError::WEBSOCKET_PROTOCOL_ERROR;
        close(StatusCode::UNKOWN_ERROR);
        priv->socket->close();
        break;
    }
    priv->payload.clear();
}

struct HttpClientSettings
{
    HttpClientSettings();

    http_parser_settings settings;
};

inline HttpClientSettings::HttpClientSettings()
{
    settings.on_header_field = WebSocketHttpClient::on_header_field;
    settings.on_header_value = WebSocketHttpClient::on_header_value;
    settings.on_headers_complete = WebSocketHttpClient::on_headers_complete;
    settings.on_message_complete = WebSocketHttpClient::on_message_complete;
}

static const HttpClientSettings httpClientSettings;

inline bool WebSocketHttpClient::execute(QByteArray &chunk)
{
    size_t nparsed = http_parser_execute(&parser,
                                         &httpClientSettings.settings,
                                         chunk.constData(),
                                         chunk.size());

    chunk.remove(0, nparsed);

    if (parser.http_errno)
        return false;

    if (statusCode() != 101)
        return false;

    if (parser.upgrade)
        return true;

    return false;
}

inline bool WebSocketHttpClient::error()
{
    return parser.http_errno;
}

inline int WebSocketHttpClient::statusCode()
{
    return parser.status_code;
}

int WebSocketHttpClient::on_header_field(http_parser *parser, const char *at,
                                      size_t length)
{
    WebSocketHttpClient *wsr = static_cast<WebSocketHttpClient *>(parser->data);
    Q_ASSERT(wsr);

    if (wsr->lastWasValue) {
        wsr->lastHeader = QByteArray(at, length);
        wsr->lastWasValue = false;
    } else {
        wsr->lastHeader.append(at, length);
    }

    return 0;
}

int WebSocketHttpClient::on_header_value(http_parser *parser, const char *at,
                                      size_t length)
{
    WebSocketHttpClient *wsr = static_cast<WebSocketHttpClient *>(parser->data);
    Q_ASSERT(wsr);

    if (wsr->lastWasValue) {
        wsr->headers.replace(wsr->lastHeader,
                             wsr->headers.value(wsr->lastHeader)
                             + QByteArray(at, length));
    } else {
        wsr->headers.insert(wsr->lastHeader, QByteArray(at, length));
        wsr->lastWasValue = true;
    }

    return 0;
}

int WebSocketHttpClient::on_headers_complete(http_parser *parser)
{
    WebSocketHttpClient *wsr = static_cast<WebSocketHttpClient *>(parser->data);
    Q_ASSERT(wsr);

    wsr->lastHeader.clear();
    wsr->lastWasValue = true;

    switch (parser->http_major) {
    case 1:
        switch (parser->http_minor) {
        case 0:
        case 1:
            break;
        default:
            return -1;
        }
        break;
    default:
        return -1;
    }

    return 0;
}

int WebSocketHttpClient::on_message_complete(http_parser *parser)
{
    WebSocketHttpClient *wsr = static_cast<WebSocketHttpClient *>(parser->data);
    Q_ASSERT(wsr);

    wsr->ready = true;

    return 0;
}

} // namespace Tufao
