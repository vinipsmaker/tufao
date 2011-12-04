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

#ifndef HTTPSERVERREQUEST_H
#define HTTPSERVERREQUEST_H

#include "httpserverresponse.h"

class QAbstractSocket;
template <class Key, class T> class QMap;

namespace Tufao {

namespace Priv {

struct HttpServerRequest;

} // namespace Priv

/*!
  \brief The Tufao::HttpServer represents a HTTP request received by
  Tufao::HttpServer.

  \sa Tufao::HttpServer
  */
class TUFAOSHARED_EXPORT HttpServerRequest : public QObject
{
    Q_OBJECT
public:
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
      */
    QByteArray method() const;

    /*!
      The request URL.

      This contains only the URL that is present in the actual HTTP request.
      If the request line is:

      \verbatim
GET /status?name=ryan HTTP/1.1\r\n
Accept: text/plain\r\n
\r\n
      \endverbatim

      Then Tufao::HttpServerRequest::url() will be "/status?name=ryan"

      \sa Tufao::Url
      */
    QByteArray url() const;

    /*!
      The HTTP headers sent by the client before the header.

      \note To save memory space, Tufao::HttpServerRequest will apply the
      QByteArray::simplified member function to every header field and value.

      \note If Tufao::HttpServerRequest receives several headers with same
      header key/name, then it will group their header values within the same
      header key and separate them by using commas.

      \sa Tufao::HttpServerRequest::trailers()
      */
    QMap<QByteArray, QByteArray> headers() const;

    /*!
      The HTTP trailers (if present). Only populated after the 'end' signal.
      */
    QMap<QByteArray, QByteArray> trailers() const;

    /*!
      The HTTP protocol version as a string.

      It can assume the following values:
        - "HTTP/1.0"
        - "HTTP/1.1"
      */
    QByteArray httpVersion() const;

    /*!
      The QAbstractSocket object associated with the connection.

      This will be a QTcpSocket object if created by Tufao::HttpServer and a
      QSslSocket if created by Tufao::HttpsServer.
      */
    QAbstractSocket *connection() const;

signals:
    /*!
      This signal is emitted when most of the data about the request was
      gathered.

      After this signal is emitted, you can safely interpret the request.
      */
    void ready(Tufao::HttpServerResponse::Options);

    /*!
      This signal is emitted each time a piece of the message body is received.

      Tufao::HttpServerRequest handles for you numerous transformations that may
      be applied by the http user agent to the entity body, so you can focus on
      parse the raw body.

      The list of transformations handled by Tufao:
        - "Content-Encoding" >> TODO
        - "Transfer-Encoding"

      The list of transformations types handled by Tufao:
        - chunked
        - gzip >> TODO
        - compress >> TODO
        - deflate >> TODO
        - identity (no transformation at all)

      The list of transformations NOT handled by Tufao (you need manually parse
      them):
        - "Content-Encoding"
      */
    void data(QByteArray data);

    /*!
      This signal is emitted exactly once for each request.

      After that, no more data signals will be emitted on the request.
      */
    void end();

    /*!
      This signal is emitted when the underlaying connection is terminated
      due to invalid request.

      Just like Tufao::HttpServerRequest::end, this signal is emitted only once
      per request, and no more data signals will fire afterwards.
      */
    void close();

private slots:
    void onReadyRead();

private:
    void clear();

    Priv::HttpServerRequest *priv;
};

} // namespace Tufao

#endif // HTTPSERVERREQUEST_H
