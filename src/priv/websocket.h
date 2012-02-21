#ifndef TUFAO_PRIV_WEBSOCKET_H
#define TUFAO_PRIV_WEBSOCKET_H

#include <QtNetwork/QAbstractSocket>
#include "../websocket.h"

namespace Tufao {
namespace Priv {

union Frame {
    char bytes[2];

    bool fin() const
    {
        return bytes[0] & 0x80;
    }

    void setFinTrue()
    {
        bytes[0] |= 0x80;
    }

    void setFinFalse()
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

    bool opcode() const
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

    void setMaskedTrue()
    {
        bytes[1] |= 0x80;
    }

    void setMaskedFalse()
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
};

namespace FrameType
{
enum FrameType
{
    // non-control frames ('\x00' to '\x07')
    CONTINUATION = '\x00',
    TEXT = '\x01',
    BINARY = '\x02',
    // control frames ('\x08' to '\x0F')
    CONNECTION_CLOSE = '\x08',
    PING = '\x09',
    PONG = '\x0A'
};
} // namespace FrameType

inline bool isControlFrame(FrameType::FrameType opcode)
{
    return opcode >= '\x08' && opcode <= '\x0F';
}

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

struct WebSocket
{
    WebSocket(Tufao::WebSocket::DeliveryType deliveryType) :
        deliveryType(deliveryType),
        messageType(Tufao::WebSocket::BINARY_MESSAGE),
        state(CLOSED),
        parsingState(PARSING_FRAME)
    {}

    Tufao::WebSocket::DeliveryType deliveryType;
    Tufao::WebSocket::MessageType messageType;

    QAbstractSocket *socket;
    QByteArray buffer;

    State state;
    bool sentMessagesAreMasked;

    ParsingState parsingState;
    quint64 remainingPayloadSize;
    quint8 maskingKey[4];
    quint8 maskingIndex;

    Frame frame;
    QByteArray fragment;
};

} // namespace Priv
} // namespace Tufao

#endif // TUFAO_PRIV_WEBSOCKET_H
