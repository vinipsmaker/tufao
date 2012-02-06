#ifndef TUFAO_PRIV_WEBSOCKET_H
#define TUFAO_PRIV_WEBSOCKET_H

#include <QtNetwork/QAbstractSocket>
#include "../websocket.h"

namespace Tufao {
namespace Priv {

// NOTE: All WebSocket integers are represented in network byte order.
// The network byte order is equal to QSysInfo::BigEndian

union Frame {
    struct
    {
        // Indicates that this is the final fragment in a message. The first
        // fragment MAY also be the final fragment.
        unsigned fin: 1;

        // MUST be 0 unless an extension is negotiated that defines meanings
        // for non-zero values. If a nonzero value is received and none of
        // the negotiated extensions defines the meaning of such a nonzero
        // value, the receiving endpoint MUST _Fail the WebSocket
        // Connection_.
        unsigned rsv1: 1;
        unsigned rsv2: 1;
        unsigned rsv3: 1;

        // Defines the interpretation of the "Payload data". If an unknown
        // opcode is received, the receiving endpoint MUST _Fail the
        // WebSocket Connection_.
        unsigned opcode: 4;

        // Defines whether the "Payload data" is masked. If set to 1, a
        // masking key is present in masking-key, and this is used to unmask
        // the "Payload data" as per Section 5.3. All frames sent from
        // client to server have this bit set to 1.
        unsigned masked: 1;

        // The length of the "Payload data", in bytes: if 0-125, that is the
        // payload length. If 126, the following 2 bytes interpreted as a
        // 16-bit unsigned integer are the payload length. If 127, the
        // following 8 bytes interpreted as a 64-bit unsigned integer (the
        // most significant bit MUST be 0) are the payload length. Multibyte
        // length quantities are expressed in network byte order. Note that
        // in all cases, the minimal number of bytes MUST be used to encode
        // the length, for example, the length of a 124-byte-long string
        // can’t be encoded as the sequence 126, 0, 124. The payload length
        // is the length of the "Extension data" + the length of the
        // "Application data". The length of the "Extension data" may be
        // zero, in which case the payload length is the length of the
        // "Application data".
        unsigned payloadLength: 7;
    } bits;
    char bytes[2];
};

namespace FrameType
{
enum
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

struct WebSocket
{
    WebSocket(Tufao::WebSocket::DeliveryType deliveryType) :
        deliveryType(deliveryType),
        state(CLOSED)
    {}

    Tufao::WebSocket::DeliveryType deliveryType;

    QAbstractSocket *socket;
    bool applyMask;
    State state;

    QByteArray buffer;
};

} // namespace Priv
} // namespace Tufao

#endif // TUFAO_PRIV_WEBSOCKET_H
