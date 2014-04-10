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

#ifndef TUFAO_PRIV_HTTPSERVERRESPONSE_H
#define TUFAO_PRIV_HTTPSERVERRESPONSE_H

#include "../httpserverresponse.h"
#include "../headers.h"

#include <QtCore/QIODevice>

namespace Tufao {

struct HttpServerResponse::Priv
{
    enum HttpResponseFormattingState
    {
        STATUS_LINE,
        HEADERS,
        MESSAGE_BODY,
        TRAILERS,
        END
    };

    Priv(QIODevice &device, Tufao::HttpServerResponse::Options options) :
        device(device),
        formattingState(STATUS_LINE),
        options(options)
    {}

    QIODevice &device;
    HttpResponseFormattingState formattingState;
    Tufao::HttpServerResponse::Options options;
    int responseStatus;
    Headers headers;

    QByteArray http10Buffer;
};

} // namespace Tufao

#endif // TUFAO_PRIV_HTTPSERVERRESPONSE_H
