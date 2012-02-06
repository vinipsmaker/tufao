#include "priv/websocket.h"
#include "httpserverrequest.h"
#include "headers.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QtEndian>
#include <QtCore/QDataStream>
#include <QtNetwork/QHostAddress>

// Writes a string without the '\0' char using function \p func
#define WRITE_STRING(func, chunk) (func)(chunk, sizeof(chunk) - 1)

static const char crlf[] = "\r\n";
#define CRLF crlf, sizeof(crlf) - 1

namespace Tufao {

WebSocket::WebSocket(DeliveryType deliveryType, QObject *parent) :
    AbstractMessageNode(parent),
    priv(new Priv::WebSocket(deliveryType))
{
}

WebSocket::~WebSocket()
{
    delete priv;
}

bool WebSocket::startClientHandshake(QAbstractSocket *socket,
                                     const QByteArray &host,
                                     const QByteArray &resource,
                                     const Headers &headers)
{
    if (!socket->isOpen())
        return false;

    priv->socket = socket;
    priv->applyMask = true;
    priv->state = Priv::CONNECTING;

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    WRITE_STRING(socket->write, "GET ");
    socket->write(resource);
    WRITE_STRING(socket->write, " HTTP/1.1\r\n");

    for (Headers::const_iterator i = headers.begin();i != headers.end();++i) {
        socket->write(i.key());
        socket->write(": ", 2);
        socket->write(i.value());
        socket->write(CRLF);
    }
    WRITE_STRING(socket->write, "Host: ");
    socket->write(host);
    WRITE_STRING(socket->write, "\r\n"

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

        socket->write(headerValue.toBase64());
    }
    WRITE_STRING(socket->write, "\r\n\r\n");

    return true;
}

bool WebSocket::startClientHandshake(QAbstractSocket *socket,
                                     const QByteArray &resource,
                                     const Headers &headers)
{
    return startClientHandshake(socket, (socket->peerAddress().toString() + ':'
                                         + socket->peerPort()).toUtf8(),
                                resource, headers);
}

bool WebSocket::startServerHandshake(const HttpServerRequest *request,
                                     const QByteArray &head,
                                     const Headers &extraHeaders)
{
    QAbstractSocket *socket = request->socket();
    Headers headers = request->headers();
    if (!headers.contains("Upgrade", "websocket")) {
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
    priv->applyMask = false;
    priv->state = Priv::OPEN;

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

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    readData(head);

    return true;
}

bool WebSocket::sendMessage(const QByteArray &msg)
{
    if (priv->state != Priv::OPEN)
        return false;

    Priv::Frame frame = standardFrame();
    frame.bits.fin = 1;
    frame.bits.opcode = Priv::FrameType::BINARY;

    writePayload(frame, msg);

    return true;
}

bool WebSocket::sendMessage(const QString &utf8Msg)
{
    if (priv->state != Priv::OPEN)
        return false;

    Priv::Frame frame = standardFrame();
    frame.bits.fin = 1;
    frame.bits.opcode = Priv::FrameType::TEXT;

    writePayload(frame, utf8Msg.toUtf8());

    return true;
}

void WebSocket::onReadyRead()
{
    readData(priv->socket->readAll());
}

void WebSocket::close()
{
    close(Priv::StatusCode::NORMAL);
}

inline Priv::Frame WebSocket::standardFrame() const
{
    Priv::Frame frame;
    frame.bits.rsv1 = 0;
    frame.bits.rsv2 = 0;
    frame.bits.rsv3 = 0;

    if (priv->applyMask)
        frame.bits.masked = 1;
    else
        frame.bits.masked = 0;

    return frame;
}

inline Priv::Frame WebSocket::controlFrame() const
{
    Priv::Frame frame = standardFrame();
    frame.bits.fin = 1;
    return frame;
}

inline void WebSocket::writePayload(Priv::Frame frame, const QByteArray &data)
{
    int size = data.size();

    if (size < 126)
        frame.bits.payloadLength = size;
    else if (size <= 65535)
        frame.bits.payloadLength = 126;
    else
        frame.bits.payloadLength = 127;

    priv->socket->write(frame.bytes, sizeof(Priv::Frame::bytes));

    if (size >=126 && size <= 65535) {
        uchar chunk[2];
        qToBigEndian(quint16(size), chunk);
        priv->socket->write(reinterpret_cast<char*>(chunk), sizeof(chunk));
    } else {
        uchar chunk[8];
        qToBigEndian(quint64(size), chunk);
        priv->socket->write(reinterpret_cast<char*>(chunk), sizeof(chunk));
    }

    if (priv->applyMask) {
        union
        {
            quint32 key;
            uchar pieces[4];
        } mask;
        mask.key = qrand();
        qToBigEndian(mask.key, mask.pieces);

        for (int i = 0;i != size;++i) {
            uchar byte = mask.pieces[i % 4] ^ data[i];
            priv->socket->write(reinterpret_cast<char*>(&byte), 1);
        }
    } else {
        priv->socket->write(data);
    }
}

inline void WebSocket::close(quint16 code)
{
    QByteArray data;
    QDataStream stream(priv->socket);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << code;

    Priv::Frame frame = controlFrame();
    frame.bits.opcode = Priv::FrameType::CONNECTION_CLOSE;

    writePayload(frame, data);
}

inline void WebSocket::readData(const QByteArray &data)
{
    priv->buffer += data;

//    Priv::Frame frame;
//    qstrncpy(frame.bytes, priv->buffer.constData(), sizeof(Priv::Frame));
//    switch (frame.bits.opcode) {
//    case Priv::FrameType::CONTINUATION:
//    case Priv::FrameType::TEXT:
//    case Priv::FrameType::BINARY:
//    case Priv::FrameType::CONNECTION_CLOSE:
//    case Priv::FrameType::PING:
//    case Priv::FrameType::PONG:
//    default:
//        // TODO: Fail the WebSocket connection
//        ;
//    }
}

} // namespace Tufao
