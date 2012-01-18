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

#ifndef TUFAO_PRIV_HTTPSERVERREQUEST_H
#define TUFAO_PRIV_HTTPSERVERREQUEST_H

#include "../headers.h"
#include "../httpserverrequest.h"
#include <QtNetwork/QAbstractSocket>
#include "http_parser.h"
#include <QtCore/QTimer>

namespace Tufao {
namespace Priv {

struct HttpServerRequest
{
    HttpServerRequest(Tufao::HttpServerRequest *request,
                      QAbstractSocket *socket) :
        socket(socket),
        lastWasValue(true),
        useTrailers(false),
        timeout(0),
        timeoutMustClose(true)
    {
        http_parser_init(&parser, HTTP_REQUEST);
        parser.data = request;
    }

    static int on_message_begin(http_parser *);
    static int on_url(http_parser *, const char *, size_t);
    static int on_header_field(http_parser *, const char *, size_t);
    static int on_header_value(http_parser *, const char *, size_t);
    static int on_headers_complete(http_parser *);
    static int on_body(http_parser *, const char *, size_t);
    static int on_message_complete(http_parser *);

    QAbstractSocket *socket;
    QByteArray buffer;
    http_parser parser;
    QByteArray lastHeader;
    bool lastWasValue;
    bool useTrailers;

    QByteArray method;
    QByteArray url;
    Tufao::HttpServerRequest::HttpVersion httpVersion;
    Headers headers;
    Headers trailers;

    int timeout;
    QTimer timer;
    bool timeoutMustClose;
};

} // namespace Priv
} // namespace Tufao

#endif // TUFAO_PRIV_HTTPSERVERREQUEST_H
