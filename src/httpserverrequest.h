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

#ifndef HTTPSERVERREQUEST_H
#define HTTPSERVERREQUEST_H

#include "httpserverresponse.h"

class QAbstractSocket;

namespace Tufao {

struct Headers;

/*!
  The Tufao::HttpServer represents a HTTP request received by Tufao::HttpServer.

  \note
  You can use it to create your own HTTP servers too, just handle the
  connections steps and pass the connection objects to Tufao::HttpServerRequest
  constructor.
  This may be useful if you want to create a threaded web server or use non
  conventional streams instead tcp sockets.

  \sa
  Tufao::HttpServer
  */
class TUFAO_EXPORT HttpServerRequest : public QObject
{
    Q_OBJECT
public:
    enum HttpVersion
    {
        HTTP_1_0,
        HTTP_1_1
    };

    /*!
      Constructs a Tufao::HttpServerRequest object.

      \p parent is passed to the QObject constructor.

      \param socket The connection used by Tufao::HttpServerRequest to receive
      HTTP messages. If you pass NULL, the object will be useless.
      */
    explicit HttpServerRequest(QAbstractSocket *socket, QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpServerRequest();

    /*!
      The request method.

      It can assume the following values:
        - "HEAD"
        - "GET"
        - "POST"
        - "PUT"
        - "DELETE"
        - "TRACE"
        - "OPTIONS"
        - "CONNECT"
        - "PATCH"
        - "COPY"
        - "LOCK"
        - "MKCOl"
        - "MOVE"
        - "PROPFIND"
        - "PROPPATCH"
        - "UNLOCK"
        - "REPORT"
        - "MKACTIVITY"
        - "CHECKOUT"
        - "MERGE"
        - "MSEARCH"
        - "NOTIFY"
        - "SUBSCRIBE"
        - "UNSUBSCRIBE"
      */
    QByteArray method() const;

    /*!
      The request URL.

      This contains only the URL that is present in the actual HTTP request.
      If the request is:

      \verbatim
GET /login?username=tux HTTP/1.1\r\n
Accept: text/plain\r\n
\r\n
      \endverbatim

      Then Tufao::HttpServerRequest::url() will be "/login?username=tux"

      \sa
      Tufao::Url
      */
    QByteArray url() const;

    /*!
      The HTTP headers sent by the client. These headers are fully populated
      when the signal Tufao::HttpServerRequest::ready signal is emitted.

      \sa
      Tufao::HttpServerRequest::trailers()
      */
    Headers headers() const;

    /*!
      The HTTP headers sent by the client. These headers are fully populated
      when the signal Tufao::HttpServerRequest::ready signal is emitted.

      \sa
      Tufao::HttpServerRequest::trailers()

      \since
      0.3
      */
    Headers &headers();

    /*!
      The HTTP trailers (if present). Only populated after the
      Tufao::HttpServerRequest::end signal.
      */
    Headers trailers() const;

    /*!
      Returns the HTTP protocol version used in the request.
      */
    HttpVersion httpVersion() const;

    /*!
      The QAbstractSocket object associated with the connection.

      This will be a QTcpSocket object if created by Tufao::HttpServer and a
      QSslSocket if created by Tufao::HttpsServer.
      */
    QAbstractSocket *socket() const;

    /*!
      Sets the timeout of new connections to \p msecs miliseconds.

      The connection will be closed when no bytes are received during \p msecs
      miliseconds.

      If you set the timeout to 0, then timeout feature will be disabled.

      By default, there is no timeout.

      You can call this function at any time.
      */
    void setTimeout(int msecs = 0);

    /*!
      Returns the current set timeout.
      */
    int timeout() const;

    /*!
      Returns the options obje that should be passed to the
      Tufao::HttpServerResponse constructor.

      \since 0.2
      */
    Tufao::HttpServerResponse::Options responseOptions() const;

signals:
    /*!
      \deprecated

      It'll be removed in 1.0 version.

      Use ready().
      */
    void ready(Tufao::HttpServerResponse::Options);

    /*!
      This signal is emitted when most of the data about the request is
      available.

      After this signal is emitted, you can safely interpret the request and the
      only missing parts may be (if any) the message body and the trailers.

      \note
      It's not safe delete this object after this signal is emitted unless you
      use a queued connection. If you want to delete this object after this
      signal was emitted, you can: You should wait for the end or close signal
        - Wait until a safe signal is emitted (end or close)
        - Close the connection (only works if you are using Tufao::HttpServer)
        - Call QObject::deleteLater()

      \sa
      Tufao::HttpServerRequest::responseOptions
      Tufao::HttpServerRequest::data
      Tufao::HttpServerRequest::end

      \since 0.2
      */
    void ready();

    /*!
      This signal is emitted each time a piece of the message body is received.

      \note
      It's not safe delete this object after this signal is emitted unless you
      use a queued connection. If you want to delete this object after this
      signal was emitted, you can: You should wait for the end or close signal
        - Wait until a safe signal is emitted (end or close)
        - Close the connection (only works if you are using Tufao::HttpServer)
        - Call QObject::deleteLater()
      */
    void data(QByteArray data);

    /*!
      This signal is emitted exactly once for each request.

      After that, no more data signals will be emitted for this session. A new
      session (if any) will be only initiated after you respond the request.
      */
    void end();

    /*!
      This signal is emitted when the underlaying connection is closed (also
      caused by invalid requests).

      Just like Tufao::HttpServerRequest::end, this signal is emitted only once
      per request, and no more data signals will fire afterwards.
      */
    void close();

    /*!
      This signal is emitted when a http upgrade is requested.

      \note
      If this signal is emitted, then the signals HttpServerRequest::ready,
      HttpServerRequest::end and HttpServerRequest::close won't be
      emitted.

      \param head The initial bytes from the new connection protocol.
      */
    void upgrade(QByteArray head);

private slots:
    void onReadyRead();
    void onTimeout();

private:
    void clearBuffer();
    void clearRequest();

    struct Priv;
    Priv *priv;

    friend struct Tufao::HttpServerRequest::Priv;
};

} // namespace Tufao

#endif // HTTPSERVERREQUEST_H
