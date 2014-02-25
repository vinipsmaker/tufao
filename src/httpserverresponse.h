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

#ifndef TUFAO_HTTPSERVERRESPONSE_H
#define TUFAO_HTTPSERVERRESPONSE_H

#include <QtCore/QObject>
#include "tufao_global.h"

class QIODevice;

namespace Tufao {

struct Headers;

/*!
  The values in this enum represents a HTTP status code. These are sent in
  the first line of a HTTP response message. You should consult external doc
  (rfc 2616) to know when to use each value. The HTTP status code most used
  is OK.

  You can use the values in this enum in
  Tufao::HttpServerResponse::writeHead(int).

  \since
  1.0
*/
enum class HttpResponseStatus
{
    // 1xx Informational
    CONTINUE                        = 100,
    SWITCHING_PROTOCOLS             = 101,
    PROCESSING                      = 102,
    CHECKPOINT                      = 103,
    // 2xx Successful
    OK                              = 200,
    CREATED                         = 201,
    ACCEPTED                        = 202,
    NON_AUTHORITATIVE_INFORMATION   = 203,
    NO_CONTENT                      = 204,
    RESET_CONTENT                   = 205,
    PARTIAL_CONTENT                 = 206,
    MULTI_STATUS                    = 207,
    ALREADY_REPORTED                = 208,
    IM_USED                         = 226,
    // 3xx Redirection
    MULTIPLE_CHOICES                = 300,
    MOVED_PERMANENTLY               = 301,
    FOUND                           = 302,
    SEE_OTHER                       = 303,
    NOT_MODIFIED                    = 304,
    USE_PROXY                       = 305,
    SWITCH_PROXY                    = 306,
    TEMPORARY_REDIRECT              = 307,
    RESUME_INCOMPLETE               = 308,
    // 4xx Client Error
    BAD_REQUEST                     = 400,
    UNAUTHORIZED                    = 401,
    PAYMENT_REQUIRED                = 402,
    FORBIDDEN                       = 403,
    NOT_FOUND                       = 404,
    METHOD_NOT_ALLOWED              = 405,
    NOT_ACCEPTABLE                  = 406,
    PROXY_AUTHENTICATION_REQUIRED   = 407,
    REQUEST_TIMEOUT                 = 408,
    CONFLICT                        = 409,
    GONE                            = 410,
    LENGTH_REQUIRED                 = 411,
    PRECONDITION_FAILED             = 412,
    REQUEST_ENTITY_TOO_LARGE        = 413,
    REQUEST_URI_TOO_LONG            = 414,
    UNSUPPORTED_MEDIA_TYPE          = 415,
    REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED              = 417,
    I_AM_A_TEAPOT                   = 418,
    UNPROCESSABLE_ENTITY            = 422,
    LOCKED                          = 423,
    FAILED_DEPENDENCY               = 424,
    UNORDERED_COLLECTION            = 425,
    UPGRADE_REQUIRED                = 426,
    PRECONDITION_REQUIRED           = 428,
    TOO_MANY_REQUESTS               = 429,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    NO_RESPONSE                     = 444,
    RETRY_WITH                      = 449,
    CLIENT_CLOSED_REQUEST           = 499,
    // 5xx Internal Server Error
    INTERNAL_SERVER_ERROR           = 500,
    NOT_IMPLEMENTED                 = 501,
    BAD_GATEWAY                     = 502,
    SERVICE_UNAVAILABLE             = 503,
    GATEWAY_TIMEOUT                 = 504,
    HTTP_VERSION_NOT_SUPPORTED      = 505,
    VARIANT_ALSO_NEGOTIATES         = 506,
    INSUFFICIENT_STORAGE            = 507,
    LOOP_DETECTED                   = 508,
    BANDWIDTH_LIMIT_EXCEEDED        = 509,
    NOT_EXTENDED                    = 510
};

/*!
  The Tufao::HttpServerResponse is used to respond to a
  Tufao::HttpServerRequest.

  A response is built of well defined parts and must be sent ordered. The order
  to send these parts are:
    - Status line: Tufao::HttpServerResponse::writeHead
    - Headers: Set them with Tufao::HttpServerResponse::headers and they will be
      automatically flushed when the first piece of body is written.
    - Message body: Use Tufao::HttpServerResponse::write or
      Tufao::HttpServerResponse::end
    - Trailers (optional): Use Tufao::HttpServerResponse::addTrailers
    - EOF: Use Tufao::HttpServerResponse::end

  \note
  In HTTP/1.0 connections, it's not possible to send the message body in chunks
  and you must use Tufao::HttpServerResponse::end to send the full body at once.
  Additionally, HTTP/1.0 connections don't support trailers too.

  \sa
  Tufao::HttpServer
  */
class TUFAO_EXPORT HttpServerResponse : public QObject
{
    Q_OBJECT
public:
    /*!
      This enum represents some aspects of a HTTP response.
      */
    enum Option
    {
        /*!
          A HTTP/1.0 response.
          */
        HTTP_1_0           = 1,
        /*!
          A HTTP/1.1 response.
          */
        HTTP_1_1           = 1 << 1,
        /*!
          The connection should use a persistent stream.

          \note
          Only supported in HTTP/1.1 connections.
          */
        KEEP_ALIVE         = 1 << 2
    };
    Q_DECLARE_FLAGS(Options, Option)

    /*!
      Constructs a Tufao::HttpServerResponse object.

      \p parent is passed to the QObject constructor.

      \param options It controls some aspects of the response.

      \param device The socket used by Tufao::HttpServerResponse to write a HTTP
      response message.

      \note
      if \p options doesn't contain HTTP_1_0 or HTTP_1_1 set, the behaviour is
      undefined. If you set both flags, the behaviour is undefined also.

      \since
      1.0
      */
    explicit HttpServerResponse(QIODevice &device,
                                Options options = Options(),
                                QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpServerResponse();

    /*!
      Returns the options passed to the object constructor.
      */
    Options options() const;

    /*!
      Change the formatting options to \p options.

      \note
      if \p options doesn't contain HTTP_1_0 or HTTP_1_1 set, the behaviour is
      undefined. If you set both flags, the behaviour is undefined also.

      \return true if successful.

      \return false if wasn't possible to change options. This can happen if the
      first chunk of data was already sent.

      \since
      1.0
     */
    bool setOptions(Options options);

    /*!
      Returns a const reference to the headers which will be sent when the first
      piece of body is written.

      \since 0.4
      */
    const Headers &headers() const;

    /*!
      Returns a reference to the headers which will be sent when the first piece
      of body is written.

      Use this reference to send custom headers.

      \note
      Change this object when the first piece of the message body was already
      written won't take any effects. However the object will retain the
      changes.
      */
    Headers &headers();

    /*!
      This method calls QAbstractSocket::flush for the object passed in the
      constructor.

      It'll do nothing and return false if the object passed in the constructor
      isn't an actual QAbstractSocket instance.

      \since
      0.3
      */
    bool flush();

signals:
    /*!
      This signal is emitted when all bytes from the HTTP response message are
      written in the device/socket (not confuse with delivered).

      Call Tufao::HttpServerResponse::end will cause this signal to be emitted.
      */
    void finished();

public slots:
    /*!
      Sends a <em>HTTP/1.1 100 Continue</em> message to the client, indicating
      that the request body should be sent.

      You should write a <em>HTTP/1.1 100 Continue</em> response to requests
      that include <em>100-continue</em> in the header <em>Expect</em> if you
      are willing to accept the request body based on the headers sent.

      If you don't want to accept the request body, you should respond it with
      an Tufao::HttpServerResponse::EXPECTATION_FAILED response status.

      The purpose of the <em>100 Continue</em> status is to decrease the
      network traffic by avoiding the transfer of data that the server would
      reject anyway.

      \warning
      It's not possible to send a <em>HTTP/1.1 100 Continue</em> to HTTP/1.0
      clients.
      */
    bool writeContinue();

    /*!
      Sends a response header to the request.

      Call this function after the first chunk of entity body data was already
      sent (calling Tufao::HttpServerResponse::write ) will have no effect.

      \param statusCode The status code is a 3-digit HTTP status code.
      \param reasonPhrase A human-readable reasonPhrase.
      \param headers The response headers.
      */
    bool writeHead(int statusCode, const QByteArray &reasonPhrase,
                   const Headers &headers);

    /*!
      This is an overloaded function.

      \sa
      Tufao::HttpServerResponse::writeHead(int, const QByteArray&, const Headers&)
      */
    bool writeHead(int statusCode, const QByteArray &reasonPhrase);

    /*!
      This is an overloaded function.

      \sa
      Tufao::HttpServerResponse::writeHead(int, const QByteArray&, const Headers&)
      */
    bool writeHead(HttpResponseStatus statusCode,
                   const QByteArray &reasonPhrase, const Headers &headers);

    /*!
      This is an overloaded function.

      \sa
      Tufao::HttpServerResponse::writeHead(int, const QByteArray&, const Headers&)
      */
    bool writeHead(HttpResponseStatus statusCode,
                   const QByteArray &reasonPhrase);

    /*!
      This is an overloaded function.

      \sa
      Tufao::HttpServerResponse::writeHead(int, const QByteArray&, const Headers&)
      */
    bool writeHead(HttpResponseStatus statusCode, const Headers &headers);

    /*!
      This is an overloaded function.

      \sa
      Tufao::HttpServerResponse::writeHead(int, const QByteArray&, const Headers&)
      */
    bool writeHead(HttpResponseStatus statusCode);

    /*!
      This sends a chunk of the response body. This method may be called
      multiple times to provide successive parts of the body.

      \note
      This is the raw HTTP body and has nothing to do with higher-level
      multi-part body encodings that may be used.

      The first time Tufao::HttpServerResponse::write is called, it will send
      the buffered headers and the first body chunk to the client. The second
      time Tufao::HttpServerResponse::write is called, it assumes you're going
      to streaming data, and sends that separately. That is, the response is
      buffered up to the first chunk of body.

      If you call this function with a empty byte array, it will do nothing.

      \note
      HTTP/1.0 user agents don't support chunked entities. To overcome this
      limitation, Tufao::HttpServerResponse will buffer the chunks and send the
      full message at once. If you want a performance boost, treat HTTP/1.0
      clients diferently.

      \sa
      operator<<(HttpServerResponse&,const QByteArray&)
      */
    bool write(const QByteArray &chunk);

    /*!
      This method adds HTTP trailing headers (a header but at the end of the
      message) to the response.

      \warning
      Trailers will only be emitted if chunked encoding is used for the
      response; if it is not (e.g., if the request was sent by a HTTP/1.0 user
      agent), they will be silently discarded.

      \note
      A server MUST NOT use the trailer for any header fields unless at least
      one of the following is true:
        - the request included a TE header field that indicates “trailers” is
          acceptable in the transfer-coding of the response;
        - the server is the origin server (your server is not a proxy or a
          tunnel) for the response, the trailer fields consist entirely of
          optional metadata, and the recipient could use the message (in a
          manner acceptable to the origin server) without receiving this
          metadata. In other words, the origin server is willing to accept the
          possibility that the trailer fields might be silently discarded along
          the path to the client.

      \sa
      Tufao::HttpServerResponse::addTrailer
      */
    bool addTrailers(const Headers &headers);

    /*!
      This method adds one HTTP trailing header (a header but at the end of the
      message) to the response.

      \warning
      Trailers will only be emitted if chunked encoding is used for the
      response; if it is not (e.g., if the request was sent by a HTTP/1.0 user
      agent), they will be silently discarded.

      \note
      A server MUST NOT use the trailer for any header fields unless at least
      one of the following is true:
        - the request included a TE header field that indicates “trailers” is
          acceptable in the transfer-coding of the response;
        - the server is the origin server (your server is not a proxy or a
          tunnel) for the response, the trailer fields consist entirely of
          optional metadata, and the recipient could use the message (in a
          manner acceptable to the origin server) without receiving this
          metadata. In other words, the origin server is willing to accept the
          possibility that the trailer fields might be silently discarded along
          the path to the client.

      \sa
      Tufao::HttpServerResponse::addTrailers
      */
    bool addTrailer(const QByteArray &headerName,
                    const QByteArray &headerValue);

    /*!
      This method signals to the server that all of the response headers and
      body has been sent; that server should consider this message complete. It
      MUST be called on each response exactly one time.

      \param chunk If specified, it is equivalent to calling
      Tufao::HttpServerResponse::write followed by
      Tufao::HttpServerResponse::end
      */
    bool end(const QByteArray &chunk = QByteArray());

private:
    struct Priv;
    Priv *priv;
};

/*!
  This overload allows you to use a HttpServerResponse object as a stream.

  \note
  You still need to call HttpServerResponse::end when done with the connection
  and HttpServerResponse::writeHead before use this operator.

  \since
  1.0
  */
inline HttpServerResponse &operator <<(HttpServerResponse &response,
                                       const QByteArray &chunk)
{
    response.write(chunk);
    return response;
}

} // namespace Tufao

#endif // TUFAO_HTTPSERVERRESPONSE_H
