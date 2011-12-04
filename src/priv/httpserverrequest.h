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

#include "../tufao_global.h"
#include <QAbstractSocket>
#include "caseinsensitivebytearraymap.h"

namespace Tufao {
namespace Priv {

enum HttpRequestParsingState
{
    READING_REQUEST_LINE,
    READING_HEADERS,
    ANALYZYING_REQUEST,
    READING_MESSAGE_BODY
};

enum MessageBodyTransformation
{
    IDENTITY = 0, // No transformation
    CHUNKED  = 1, // Series of chunks
    // TODO: implement the following:
    GZIP     = 1 << 1,
    COMPRESS = 1 << 2,
    DEFLATE  = 1 << 3
};

Q_DECLARE_FLAGS(MessageBodyTransformations, MessageBodyTransformation)

struct HttpServerRequest
{
    HttpServerRequest(QAbstractSocket *socket) :
        socket(socket),
        parsingState(READING_REQUEST_LINE),
        messageBodyTransformations(IDENTITY),
        messageLength(0)
    {}

    QAbstractSocket *socket;
    QByteArray buffer;
    HttpRequestParsingState parsingState;

    QByteArray method;
    QByteArray url;
    QByteArray httpVersion;
    CaseInsensitiveByteArrayMap headers;
    CaseInsensitiveByteArrayMap trailers;

    MessageBodyTransformations messageBodyTransformations;
    // Only used if CHUNKED transformation is NOT present
    int messageLength;
};

} // namespace Priv
} // namespace Tufao

#endif // TUFAO_PRIV_HTTPSERVERREQUEST_H
