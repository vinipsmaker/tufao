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

#include <QObject>
#include "tufao_global.h"

class QAbstractSocket;
template <class Key, class T> class QMap;

namespace Tufao {

/*!
  \brief The Tufao::HttpServer represents a HTTP request received by
  Tufao::HttpServer

  \sa Tufao::HttpServer
  */
class TUFAOSHARED_EXPORT HttpServerRequest : public QObject
{
    Q_OBJECT
public:
    /*!
      This enum describes the communication errors that can occur.
      */
    enum Error {
        /*!
          It indicates that the client has closed the underlaying connection
          prematurely.

          All incoming connections have a default timeout of 2 minutes.
          */
        ABORTED,
        /*!
          It indicates that the underlaying connection timed out.
          */
        TIMEOUT
    };

    /*!
      Constructs a Tufao::HttpServerRequest object.

      \p parent is passed to the QObject constructor.
      */
    explicit HttpServerRequest(QObject *parent = 0);

    /*!
      The request method.

      The allowed methods are:
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

      \sa Tufao::HttpServerRequest::trailers()
      */
    QMap<QByteArray, QByteArray> headers() const;

    /*!
      The HTTP trailers (if present). Only populated after the 'end' signal.
      */
    QMap<QByteArray, QByteArray> trailers() const;

    /*!
      The HTTP protocol version as a string.

      Allowed versions are "HTTP/1.0" and "HTTP/1.1"
      */
    QByteArray httpVersion() const;

    /*!
      The QAbstractSocket object associated with the connection.

      This will be a QTcpSocket object if created by Tufao::HttpServer and a
      QSslSocket if created by Tufao::HttpsServer.
      */
    QAbstractSocket *connection();

signals:
    /*!
      This signal is emitted each time a piece of the message body is received.
      */
    void data(QByteArray data);

    /*!
      This signal is emitted exactly once for each request.

      After that, no more data signals will be emitted on the request.
      */
    void end();

    /*!
      This signal is emitted when the underlaying connection is terminated
      before Tufao::HttpServerRequest::end is emitted or able to flush.

      \param error It indicates the reason for the timeout.

      Just like Tufao::HttpServerRequest::end, this signal is emitted only once
      per request, and no more data signals will fire afterwards.

      \note Tufao::HttpServerRequest::close can fire after end, but not vice
      versa.
      */
    void close(Tufao::HttpServerRequest::Error error);
};

} // namespace Tufao

#endif // HTTPSERVERREQUEST_H
