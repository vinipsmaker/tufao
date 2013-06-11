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

#ifndef TUFAO_PRIV_WEBSOCKET_H
#define TUFAO_PRIV_WEBSOCKET_H

#include "../websocket.h"
#include "http_parser.h"

#include <QtNetwork/QAbstractSocket>
#include <QtCore/QtEndian>

#if defined(NO_ERROR) && defined(_WIN32)
# define TUFAO_WINERROR_WORKAROUND
# undef NO_ERROR
#endif

namespace Tufao {

struct HttpClientSettings;

namespace FrameType
{
enum FrameType
{
    // data frames ('\x00' to '\x07')
    CONTINUATION = '\x00',
    TEXT = '\x01',
    BINARY = '\x02',
    // control frames ('\x08' to '\x0F')
    CONNECTION_CLOSE = '\x08',
    PING = '\x09',
    PONG = '\x0A'
};
} // namespace FrameType

namespace StatusCode {
enum
{
    NORMAL                = 1000,
    GOING_AWAY            = 1001,
    PROTOCOL_ERROR        = 1002,
    UNSUPPORTED_DATA_TYPE = 1003,
    STATUS_CODE_ABSENT    = 1005,
    INVALID_DATA          = 1007,
    UNKOWN_ERROR          = 1008,
    MESSAGE_TOO_BIG       = 1009,
    EXTENSION_REQUIRED    = 1010,
    UNEXPECTED_CONDITION  = 1011
};
} // namespace StatusCode

class WebSocketHttpClient
{
public:
    WebSocketHttpClient() :
        ready(false),
        lastWasValue(true)
    {
        http_parser_init(&parser, HTTP_RESPONSE);
        parser.data = this;
    }

    /*!
      Return true if the request is complete and got a Upgrade response.
      */
    bool execute(QByteArray &chunk);

    bool error();

    int statusCode();

    Headers headers;

    /*!
      true if the request was completed.
      */
    bool ready;

private:
    static int on_header_field(http_parser *, const char *, size_t);
    static int on_header_value(http_parser *, const char *, size_t);
    static int on_headers_complete(http_parser *);
    static int on_message_complete(http_parser *);

    http_parser parser;
    QByteArray lastHeader;
    bool lastWasValue;

    friend struct HttpClientSettings;
};

struct WebSocketClientNode
{
    WebSocketHttpClient response;

    // request info
    Headers headers;
    QByteArray resource;

    QByteArray expectedWebSocketAccept;
};

struct WebSocket::Priv
{
    union Frame
    {
        char bytes[2];

        bool fin() const
        {
            return bytes[0] & 0x80;
        }

        void setFin()
        {
            bytes[0] |= 0x80;
        }

        void unsetFin()
        {
            bytes[0] &= 0x7F;
        }

        bool rsv1() const
        {
            return bytes[0] & 0x40;
        }

        bool rsv2() const
        {
            return bytes[0] & 0x20;
        }

        bool rsv3() const
        {
            return bytes[0] & 0x10;
        }

        quint8 opcode() const
        {
            return bytes[0] & 0xF;
        }

        void setOpcode(quint8 opcode)
        {
            bytes[0] &= 0xF0;
            bytes[0] |= opcode & 0xF;
        }

        bool masked() const
        {
            return bytes[1] & 0x80;
        }

        void setMasked()
        {
            bytes[1] |= 0x80;
        }

        void unsetMasked()
        {
            bytes[1] &= 0x7F;
        }

        quint8 payloadLength() const
        {
            return bytes[1] & 0x7F;
        }

        void setPayloadLength(quint8 length)
        {
            bytes[1] &= 0x80;
            bytes[1] |= length & 0x7F;
        }

        bool isControlFrame()
        {
            return bytes[0] & 0x8;
        }

        bool isDataFrame()
        {
            return !isControlFrame();
        }

        void writePayload(QAbstractSocket *socket, bool isClientNode, const QByteArray &data)
        {
            int size = data.size();

            if (size < 126)
                setPayloadLength(size);
            else if (size <= 65535)
                setPayloadLength(126);
            else
                setPayloadLength(127);

            socket->write(bytes, 2);

            if (size >= 126 && size <= 65535) {
                uchar chunk[2];
                qToBigEndian(quint16(size), chunk);
                socket->write(reinterpret_cast<char*>(chunk), sizeof(chunk));
            } else if (size > 65535) {
                uchar chunk[8];
                qToBigEndian(quint64(size), chunk);
                socket->write(reinterpret_cast<char*>(chunk), sizeof(chunk));
            }

            if (isClientNode) {
                union
                {
                    quint32 key;
                    uchar pieces[4];
                } mask;
                mask.key = qrand();
                socket->write(reinterpret_cast<char*>(mask.pieces), 4);

                for (int i = 0;i != size;++i) {
                    uchar byte = mask.pieces[i % 4] ^ data[i];
                    socket->write(reinterpret_cast<char*>(&byte), 1);
                }
            } else {
                socket->write(data);
            }
        }

        static Frame standardFrame(bool isClientNode)
        {
            Priv::Frame frame;
            frame.bytes[0] = 0;
            frame.bytes[1] = 0;

            if (isClientNode)
                frame.setMasked();

            return frame;
        }

        static Frame controlFrame(bool isClientNode)
        {
            Priv::Frame frame = standardFrame(isClientNode);
            frame.setFin();
            return frame;
        }
    };

    inline bool isControlFrame(FrameType::FrameType opcode)
    {
        return opcode & 0x8;
    }

    inline bool isDataFrame(FrameType::FrameType opcode)
    {
        return !isControlFrame(opcode);
    }

    enum State
    {
        CONNECTING,
        OPEN,
        CLOSING,
        CLOSED
    };

    enum ParsingState
    {
        PARSING_FRAME,
        PARSING_SIZE_16BIT,
        PARSING_SIZE_64BIT,
        PARSING_MASKING_KEY,
        PARSING_PAYLOAD_DATA
    };

    Priv() :
        messageType(WebSocketMessageType::BINARY_MESSAGE),
        lastError(WebSocketError::NO_ERROR),
        state(CLOSED),
        parsingState(PARSING_FRAME),
        clientNode(NULL)
    {}

    ~Priv()
    {
        if (clientNode)
            delete clientNode;
    }

    WebSocketMessageType messageType;
    WebSocketError lastError;

    QAbstractSocket *socket;
    QByteArray buffer;

    State state;
    bool isClientNode;

    ParsingState parsingState;
    quint64 remainingPayloadSize;
    quint8 maskingKey[4];
    quint8 maskingIndex;

    // Used by client nodes only during the opening handshake
    WebSocketClientNode *clientNode;

    // CURRENT frame:
    Frame frame;
    QByteArray payload;

    // Used in fragmented messages
    quint8 fragmentOpcode;
    QByteArray fragment;
};

inline bool hasValueCaseInsensitively(const QList<QByteArray> &values,
                                      const QByteArray &value)
{
    for (int i = 0;i != values.size();++i) {
        if (qstricmp(values[i].constData(), value.constData()) == 0)
            return true;
    }
    return false;
}

} // namespace Tufao

#if defined(TUFAO_WINERROR_WORKAROUND)
# define NO_ERROR 0L
# undef TUFAO_WINERROR_WORKAROUND
#endif

#endif // TUFAO_PRIV_WEBSOCKET_H
