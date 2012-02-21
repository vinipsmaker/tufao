#include "priv/websocket.h"
#include "httpserverrequest.h"
#include "headers.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QtEndian>
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
    priv->sentMessagesAreMasked = true;
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
    priv->sentMessagesAreMasked = false;
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

    if (head.size())
        readData(head);

    return true;
}

bool WebSocket::sendMessage(const QByteArray &msg)
{
    if (priv->state != Priv::OPEN)
        return false;

    Priv::Frame frame = standardFrame();
    frame.setFinTrue();
    frame.setOpcode(Priv::FrameType::BINARY);

    writePayload(frame, msg);

    return true;
}

bool WebSocket::sendMessage(const QString &utf8Msg)
{
    if (priv->state != Priv::OPEN)
        return false;

    Priv::Frame frame = standardFrame();
    frame.setFinTrue();
    frame.setOpcode(Priv::FrameType::TEXT);

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
    frame.bytes[0] = 0;
    frame.bytes[1] = 0;

    if (priv->sentMessagesAreMasked)
        frame.setMaskedTrue();
    else
        frame.setMaskedFalse();

    return frame;
}

inline Priv::Frame WebSocket::controlFrame() const
{
    Priv::Frame frame = standardFrame();
    frame.setFinTrue();
    return frame;
}

inline void WebSocket::writePayload(Priv::Frame frame, const QByteArray &data)
{
    int size = data.size();

    if (size < 126)
        frame.setPayloadLength(size);
    else if (size <= 65535)
        frame.setPayloadLength(126);
    else
        frame.setPayloadLength(127);

    priv->socket->write(frame.bytes, 2);

    if (size >= 126 && size <= 65535) {
        uchar chunk[2];
        qToBigEndian(quint16(size), chunk);
        priv->socket->write(reinterpret_cast<char*>(chunk), sizeof(chunk));
    } else if (size > 65535) {
        uchar chunk[8];
        qToBigEndian(quint64(size), chunk);
        priv->socket->write(reinterpret_cast<char*>(chunk), sizeof(chunk));
    }

    if (priv->sentMessagesAreMasked) {
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
    uchar chunk[2];
    qToBigEndian(code, chunk);

    QByteArray data(reinterpret_cast<char*>(chunk), 2);

    Priv::Frame frame = controlFrame();
    frame.setOpcode(Priv::FrameType::CONNECTION_CLOSE);

    writePayload(frame, data);
}

inline void WebSocket::readData(const QByteArray &data)
{
    priv->buffer += data;
    switch (priv->state) {
    case Priv::CONNECTING:
        // TODO: this is used soon after startClientHandshake
        break;
    case Priv::OPEN:
        parseBuffer();
        break;
    case Priv::CLOSING:
        // TODO
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

    if (priv->frame.payloadLength() == 126) {
        priv->parsingState = Priv::PARSING_SIZE_16BIT;
    } else if (priv->frame.payloadLength() == 127) {
        priv->parsingState = Priv::PARSING_SIZE_64BIT;
    } else {
        priv->remainingPayloadSize = priv->frame.payloadLength();

        if (!priv->sentMessagesAreMasked)
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

    if (!priv->sentMessagesAreMasked)
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

    if (!priv->sentMessagesAreMasked)
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
    if (!priv->buffer.size())
        return false;

    QByteArray chunk = priv->buffer.left(priv->remainingPayloadSize);
    priv->buffer.remove(0, chunk.size());
    priv->remainingPayloadSize -= chunk.size();

    if (priv->deliveryType == DELIVER_PARTIAL_FRAMES) {
        decodeFragment(chunk);
        emit newMessage(chunk);
    } else
        priv->fragment += chunk;

    if (priv->remainingPayloadSize)
        return false;

    if (priv->frame.fin()
            && priv->deliveryType == DELIVER_FULL_FRAMES) {
        decodeFragment(priv->fragment);
        emit newMessage(priv->fragment);
        priv->fragment.clear();
    }

    priv->parsingState = Priv::PARSING_FRAME;

    return true;
}

inline void WebSocket::decodeFragment(QByteArray &fragment)
{
    if (priv->sentMessagesAreMasked)
        return;

    for (int i = 0;i != fragment.size();++i) {
        fragment[i] = fragment[i] ^ priv->maskingKey[priv->maskingIndex % 4];
        priv->maskingIndex += 1;
    }
}

} // namespace Tufao
