#ifndef TUFAO_WEBSOCKET_H
#define TUFAO_WEBSOCKET_H

#include "abstractmessagenode.h"
#include "headers.h"

class QAbstractSocket;

namespace Tufao {
namespace Priv {

struct WebSocket;
union Frame;

} // namespace Priv

class HttpServerRequest;

class WebSocket : public AbstractMessageNode
{
    Q_OBJECT
public:
    enum DeliveryType
    {
        DELIVER_FULL_FRAMES,
        DELIVER_PARTIAL_FRAMES
    };

    enum MessageType
    {
        TEXT_MESSAGE,
        BINARY_MESSAGE
    };

    explicit WebSocket(DeliveryType deliveryType = DELIVER_FULL_FRAMES,
                       QObject *parent = 0);
    ~WebSocket();

    /*!
      \note
      Optional headers such as Origin aren't added by default. If you want to
      use one of the headers, you should add them explicitly. The list of
      optional headers is the following:
        - Origin: RFC 6454. If your software is a browser client, this field is
          mandatory, not optional.
        - Sec-WebSocket-Protocol: If present, this value indicates one or more
          comma-separated subprotocol the client wishes to speak, ordered by
          preference.
      */
    bool startClientHandshake(QAbstractSocket *socket,
                              const QByteArray &host,
                              const QByteArray &resource,
                              const Headers &headers = Headers());

    /*!
      This is an overloaded function.

      It uses \p socket to fill the "Host" header.
      */
    bool startClientHandshake(QAbstractSocket *socket,
                              const QByteArray &resource,
                              const Headers &headers = Headers());

    /*!
      \note
      You should call this function only after \p request emitts the
      Tufao::HttpServerRequest::upgrade signal.

      \note
      WebSocketNode hold any info about the request. If you need info, get
      yourself. A list of interesting headers may be:
        - Origin: Compare the Origin to the Host to block malicious scripts
          coming from web browsers.
        - Cookie
        - Sec-WebSocket-Protocol

      \note
      If you want to perform additional client authentication, you should
      start the handshake only after the authentication occurs. You can use the
      401 status code with "WWW-Authenticate" header to perform the
      authentication.

      \note
      You can use a 3xx status code to redirect the client.

      \note
      This method may write to the socket and fail to create a WebSocket
      connection, returning false, but it won't close the connection.

      \param headers extra headers that you can send to the client, such as
      "Sec-WebSocket-Protocol" to specify which subprotocol to use
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
    MessageType messagesType();

    void close();

signals:
    void closeRequest();

public slots:
    bool sendMessage(const QByteArray &msg);
    bool sendBinaryMessage(const QByteArray &msg);
    bool sendUtf8Message(const QByteArray &msg);

private slots:
    void onReadyRead();

private:
    Priv::Frame standardFrame() const;
    Priv::Frame controlFrame() const;

    void writePayload(Priv::Frame frame, const QByteArray &data);
    void close(quint16 code);

    void readData(const QByteArray &data);
    void parseBuffer();
    bool parseFrame();
    bool parseSize16();
    bool parseSize64();
    bool parseMaskingKey();
    bool parsePayloadData();

    void decodeFragment(QByteArray &fragment);

    Priv::WebSocket *priv;
};

} // namespace Tufao

#endif // TUFAO_WEBSOCKET_H
