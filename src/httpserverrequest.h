/*
  Copyright (c) 2012, 2013 Vinícius dos Santos Oliveira

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

#ifndef TUFAO_HTTPSERVERREQUEST_H
#define TUFAO_HTTPSERVERREQUEST_H

#include "httpserverresponse.h"

class QAbstractSocket;
class QUrl;

namespace Tufao {

struct Headers;

enum class HttpVersion
{
    HTTP_1_0,
    HTTP_1_1
};

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

    /*!
      Constructs a Tufao::HttpServerRequest object.

      \p parent is passed to the QObject constructor.

      \param socket The connection used by Tufao::HttpServerRequest to receive
      HTTP messages. If you pass NULL, the object will be useless.

      \since
      1.0
      */
    explicit HttpServerRequest(QAbstractSocket &socket, QObject *parent = 0);

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
        - "SEARCH"
        - "UNLOCK"
        - "REPORT"
        - "MKACTIVITY"
        - "CHECKOUT"
        - "MERGE"
        - "M-SEARCH"
        - "NOTIFY"
        - "SUBSCRIBE"
        - "UNSUBSCRIBE"
        - "PURGE"
      */
    QByteArray method() const;

    /*!
      Sets the request URL.

      \sa
      UrlRewriterHandler

      \since
      1.0
     */
    void setUrl(const QUrl &url);

    /*!
      The request URL.

      This contains only the URL that is present in the actual HTTP request.
      If the request is:

          GET /login?username=tux HTTP/1.1\r\n
          Accept: text/plain\r\n
          \r\n

      Then Tufao::HttpServerRequest::url() will be constructed with
      "/login?username=tux".

      \since
      1.0
      */
    QUrl url() const;

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

      Trailers are headers sent after the body. Some headers can't be computed
      before the full body is generated. The solution to decrease the network
      latency is send the body before the associated metadata using the trailers
      technique.
      */
    Headers trailers() const;

    /*!
      Returns the HTTP protocol version used in the request.
      */
    HttpVersion httpVersion() const;

    /*!
      Read the request's body.

      \return
      This request's object will buffer every piece of body received. After call
      this function, the buffered content is returned and the buffer is cleared.

      \note
      If you only call this function after end() signal, the returned object
      will be the entire body of the request. Call this function when the data()
      signal is emitted and save the body to disk if you expect to receive
      requests with bodies larger than available RAM.

      \sa
      data()
      end()

      \since
      1.0
     */
    QByteArray readBody();

    /*!
      The QAbstractSocket object associated with the connection.

      This will be a QTcpSocket object if created by Tufao::HttpServer and a
      QSslSocket if created by Tufao::HttpsServer.

      \since
      1.0
      */
    QAbstractSocket &socket() const;

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

    /*!
      Returns the user data as set in setCustomData.

      \note
      This data will be erased upon a new request.

      \sa
      setCustomData

      \since 1.0
     */
    QVariant customData() const;

    /*!
      Sets the custom data to \p data.

      The custom data is a convenience method to allow users of
      HttpServerRequest to store some data in some requests. It's used in
      Tufao::HttpServerRequestRouter to pass the list of captured texts in the
      url to the subsequent handlers.

      \note
      This data will be erased upon a new request.

      \since 1.0
     */
    void setCustomData(const QVariant &data);

public slots:
    /*!
      This function exists to support HTTP pipelining.

      HTTP protocol allows several requests to be sent before the reply to the
      first one is issued. HttpServerRequest will stop processing its internal
      buffer once the end of some message is reached. You should call this
      function once you issue the reply to a message.

      HttpServer will automatically connect the HttpServerResponse::finished
      signal to this slot, so you shouldn't need to worry about anything if
      you're using Tufão's standard abstractions.

      \since 1.4
     */
    void resume();

signals:
    /*!
      This signal is emitted when most of the data about the request is
      available.

      After this signal is emitted, you can safely interpret the request and the
      only missing parts may be (if any) the message body and the trailers.

      \note
      __This signal is unsafe__ (read this: \ref safe-signal)!

      \warning
      Right before emit this signal, HttpServerRequest object will disconnect
      any slot connected to the signals listed below. This behaviour was chosen
      to allow you to think about the single HTTP session without worry about
      the Tufão behaviour of reuse the same objects to the same connections.
        - HttpServerRequest::data
        - HttpServerRequest::end

      \sa
      Tufao::HttpServerRequest::responseOptions
      Tufao::HttpServerRequest::data
      Tufao::HttpServerRequest::end

      \since 0.2
      */
    void ready();

    /*!
      This signal is emitted each time a piece of the message body is received.

      Use readBody() to consume the data.

      \note
      __This signal is unsafe__ (read this: \ref safe-signal)!

      \since
      1.0
      */
    void data();

    /*!
      This signal is emitted exactly once for each request.

      Use readBody() to access the request's body and trailers() to access the
      headers sent after the body.

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

      The body is set to the initial bytes from the new connection session
      (under the new protocol).

      \since
      1.0
      */
    void upgrade();

private slots:
    void onReadyRead();
    void onTimeout();

private:
    void clearBuffer();
    void clearRequest();

    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPSERVERREQUEST_H
