/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HTTPSERVERRESPONSE_H
#define HTTPSERVERRESPONSE_H

#include <QObject>
#include "tufao_global.h"

template <class Key, class T> class QMap;

namespace Tufao {

/*!
  \brief The Tufao::HttpServerResponse is used to respond to a
  Tufao::HttpServerRequest.

  \sa Tufao::HttpServer
  */
class TUFAOSHARED_EXPORT HttpServerResponse : public QObject
{
    Q_OBJECT
public:
    /*!
      Constructs a Tufao::HttpServerResponse object.

      \p parent is passed to the QObject constructor.
      */
    explicit HttpServerResponse(QObject *parent = 0);

    /*!
      When using implicit headers (not calling
      Tufao::HttpServerResponse::writeHead explicitly), this property controls
      the status code that will be send to the client when the headers get
      flushed.
      */
    int statusCode() const;

    /*!
      Sets a single header value for implicit headers. If this header already
      exists in the to-be-sent headers, it's value will be replaced.

      \param name The header key.
      \param value The header value.
      */
    void setHeader(const QByteArray &name, const QByteArray &value);

    /*!
      Reads out a header that's already been queued but not sent to the client.

      \note The name is case insensitive. This can only be called before headers
      get implicitly flushed (when you start to send the entity body).
      */
    QByteArray header(const QByteArray &name);

    /*!
      Removes a header that's queued for implicit sending.
      */
    void removeHeader(const QByteArray &name);

public slots:
    /*!
      Sends a <em>HTTP/1.1 100 Continue</em> message to the client, indicating
      that the request body should be sent.
      */
    void writeContinue();

    /*!
      Sends a response header to the request.

      \param statusCode The status code is a 3-digit HTTP status code.
      \param reasonPhrase A human-readable reasonPhrase.
      \param headers The response headers.
      */
    void writeHead(int statusCode, const QByteArray &reasonPhrase,
                   const QMap<QByteArray, QByteArray> &headers);

    /*!
      This is an overloaded function.

      \sa
      Tufao::HttpServerResponse::writeHead(int, const QByteArray&, const QMap<QByteArray,QByteArray>&)
      */
    void writeHead(int statusCode, const QMap<QByteArray, QByteArray> &headers);

    /*!
      This is an overloaded function.

      \sa
      Tufao::HttpServerResponse::writeHead(int, const QByteArray&, const QMap<QByteArray,QByteArray>&)
      */
    void writeHead(int statusCode);

    /*!
      If this method is called and Tufao::HttpServerResponse::writeHead has not
      been called, it will switch to implicit header mode and flush the implicit
      headers.

      This sends a chunk of the response body. This method may be called
      multiple times to provide successive parts of the body.

      \note This is the raw HTTP body and has nothing to do with higher-level
      multi-part body encodings that may be used.

      The first time Tufao::HttpServerResponse::write is called, it will send
      the buffered header information and the first body to the client. The
      second time Tufao::HttpServerResponse::write is called, Tufao assumes
      you're going to be streaming data, and sends that separately. That is, the
      response is buffered up to the first chunk of body.
      */
    void write(const QByteArray &chunk);

    /*!
      This method adds HTTP trailing headers (a header but at the end of the
      message) to the response.

      \warning Trailers will only be emitted if chunked encoding is used for the
      response; if it is not (e.g., if the request was HTTP/1.0), they will be
      silently discarded.

      \note Note that HTTP requires the Trailer header to be sent if you intend
      to emit trailers, with a list of the header fields in its value. E.g.,
      \code
response->setHeader("Trailer", "TraceInfo");
response->writeHead(200);
response->addTrailer("Content-MD5", "41229d92877c8260823914c0c9afc4f4");
response->end();
      \endcode
      */
    void addTrailers(const QMap<QByteArray, QByteArray> &headers);

    /*!
      This method adds one HTTP trailing header (a header but at the end of the
      message) to the response.

      \sa Tufao::HttpServerResponse::addTrailers
      */
    void addTrailer(const QByteArray &headerName,
                    const QByteArray &headerValue);

    /*!
      This method signals to the server that all of the response headers and
      body has been sent; that server should consider this message complete. The
      method, response.end(), MUST be called on each response.

      \param chunk If specified, it is equivalent to calling
      Tufao::HttpServerResponse::write followed by
      Tufao::HttpServerResponse::end
      */
    void end(const QByteArray &chunk = QByteArray());
};

} // namespace Tufao

#endif // HTTPSERVERRESPONSE_H
