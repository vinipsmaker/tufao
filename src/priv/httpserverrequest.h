/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
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

#include <boost/http/reader/request.hpp>

#include "../headers.h"
#include "../httpserverrequest.h"

#include <QtNetwork/QAbstractSocket>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

namespace Tufao {

namespace http = boost::http;
namespace reader = http::reader;
namespace asio = boost::asio;

struct HttpServerRequest::Priv
{
    enum Signal
    {
        READY   = 1,
        DATA    = 1 << 1,
        END     = 1 << 2
    };
    Q_DECLARE_FLAGS(Signals, Signal)

    Priv(QAbstractSocket &socket) :
        socket(socket),
        responseOptions(0),
        timeout(0)
    {
        timer.setSingleShot(true);
    }

    QAbstractSocket &socket;
    QByteArray buffer;
    reader::request parser;
    QByteArray lastHeader;
    QByteArray body;

    QByteArray method;
    QUrl url;
    Tufao::HttpVersion httpVersion;
    Headers headers;
    Headers trailers;
    Tufao::HttpServerResponse::Options responseOptions;
    QVariant customData;

    int timeout;
    QTimer timer;
};

} // namespace Tufao

#endif // TUFAO_PRIV_HTTPSERVERREQUEST_H
