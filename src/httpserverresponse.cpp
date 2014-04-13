/*  This file is part of the Tufão project
    Copyright (C) 2011-2013 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "priv/httpserverresponse.h"
#include "priv/reasonphrase.h"

#include <QtNetwork/QAbstractSocket>

static const char crlf[] = "\r\n";
#define CRLF crlf, sizeof(crlf) - 1

static const char lastChunk[] = "0\r\n";
#define LAST_CHUNK lastChunk, sizeof(lastChunk) - 1

namespace Tufao {

HttpServerResponse::HttpServerResponse(QIODevice &device, Options options,
                                       QObject *parent) :
    QObject(parent),
    priv(new Priv(device, options))
{
}

HttpServerResponse::~HttpServerResponse()
{
    delete priv;
}

HttpServerResponse::Options HttpServerResponse::options() const
{
    return priv->options;
}

bool HttpServerResponse::setOptions(Options options)
{
    if (priv->formattingState != Priv::STATUS_LINE)
        return false;

    priv->options = options;
    return true;
}

const Headers &HttpServerResponse::headers() const
{
    return priv->headers;
}

Headers &HttpServerResponse::headers()
{
    return priv->headers;
}

bool HttpServerResponse::flush()
{
    QAbstractSocket *socket = qobject_cast<QAbstractSocket *>(&priv->device);
    if (!socket)
        return false;

    return socket->flush();
}

bool HttpServerResponse::writeContinue()
{
    if (priv->formattingState != Priv::STATUS_LINE
            || priv->options.testFlag(HttpServerResponse::HTTP_1_0))
        return false;

    if (priv->options.testFlag(HTTP_1_1)) {
        static const char chunk[] = "HTTP/1.1 100 Continue\r\n\r\n";
        priv->device.write(chunk, sizeof(chunk) - 1);
    } else {
        static const char chunk[] = "HTTP/1.0 100 Continue\r\n\r\n";
        priv->device.write(chunk, sizeof(chunk) - 1);
    }
    return true;
}

bool HttpServerResponse::writeHead(int statusCode,
                                   const QByteArray &reasonPhrase,
                                   const Headers &headers)
{
    if (priv->formattingState != Priv::STATUS_LINE)
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0)) {
        static const char chunk[] = "HTTP/1.0 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    } else {
        static const char chunk[] = "HTTP/1.1 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    }

    priv->device.write(QByteArray::number(statusCode));
    priv->device.write(" ", 1);
    priv->device.write(reasonPhrase);
    priv->device.write(CRLF);

    for (Headers::const_iterator i = headers.constBegin()
         ;i != headers.end();++i) {
        priv->device.write(i.key());
        priv->device.write(": ", 2);
        priv->device.write(i.value());
        priv->device.write(CRLF);
    }
    priv->formattingState = Priv::HEADERS;
    return true;
}

bool HttpServerResponse::writeHead(int statusCode, const QByteArray &reasonPhrase)
{
    if (priv->formattingState != Priv::STATUS_LINE)
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0)) {
        static const char chunk[] = "HTTP/1.0 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    } else {
        static const char chunk[] = "HTTP/1.1 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    }

    priv->device.write(QByteArray::number(statusCode));
    priv->device.write(" ", 1);
    priv->device.write(reasonPhrase);
    priv->device.write(CRLF);
    priv->formattingState = Priv::HEADERS;
    return true;
}

bool HttpServerResponse::writeHead(HttpResponseStatus statusCode,
                                   const QByteArray &reasonPhrase,
                                   const Headers &headers)
{
    return writeHead(int(statusCode), reasonPhrase, headers);
}

bool HttpServerResponse::writeHead(HttpResponseStatus statusCode,
                                   const QByteArray &reasonPhrase)
{
    return writeHead(int(statusCode), reasonPhrase);
}

bool HttpServerResponse::writeHead(HttpResponseStatus statusCode,
                                   const Headers &headers)
{
    if (priv->formattingState != Priv::STATUS_LINE)
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0)) {
        static const char chunk[] = "HTTP/1.0 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    } else {
        static const char chunk[] = "HTTP/1.1 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    }

    priv->device.write(QByteArray::number(int(statusCode)));
    priv->device.write(" ", 1);
    priv->device.write(reasonPhrase(statusCode));
    priv->device.write(CRLF);

    for (Headers::const_iterator i = headers.constBegin()
         ;i != headers.end();++i) {
        priv->device.write(i.key());
        priv->device.write(": ", 2);
        priv->device.write(i.value());
        priv->device.write(CRLF);
    }
    priv->formattingState = Priv::HEADERS;
    return true;
}

bool HttpServerResponse::writeHead(HttpResponseStatus statusCode)
{
    if (priv->formattingState != Priv::STATUS_LINE)
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0)) {
        static const char chunk[] = "HTTP/1.0 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    } else {
        static const char chunk[] = "HTTP/1.1 ";
        priv->device.write(chunk, sizeof(chunk) - 1);
    }

    priv->device.write(QByteArray::number(int(statusCode)));
    priv->device.write(" ", 1);
    priv->device.write(reasonPhrase(statusCode));
    priv->device.write(CRLF);
    priv->formattingState = Priv::HEADERS;
    return true;
}

bool HttpServerResponse::write(const QByteArray &chunk)
{
    if (!chunk.size())
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0)) {
        switch (priv->formattingState) {
        case Priv::STATUS_LINE:
        case Priv::END:
            return false;
        case Priv::HEADERS:
        default:
            priv->http10Buffer.push_back(chunk);
            return true;
        }
    }

    switch (priv->formattingState) {
    case Priv::STATUS_LINE:
    case Priv::END:
    case Priv::TRAILERS:
        return false;
    case Priv::HEADERS:
    {
        if (priv->options.testFlag(HttpServerResponse::KEEP_ALIVE)) {
            static const char key[] = "Connection", value[] = "keep-alive";
            priv->headers.replace(QByteArray::fromRawData(key, sizeof(key) - 1),
                                  QByteArray::fromRawData(value,
                                                          sizeof(value) - 1));
        } else {
            static const char key[] = "Connection", value[] = "close";
            priv->headers.replace(QByteArray::fromRawData(key, sizeof(key) - 1),
                                  QByteArray::fromRawData(value,
                                                          sizeof(value) - 1));
        }
        priv->headers.insert("Transfer-Encoding", "chunked");

        for (Headers::iterator i = priv->headers.begin()
             ;i != priv->headers.end();++i) {
            priv->device.write(i.key());
            priv->device.write(": ", 2);
            priv->device.write(i.value());
            priv->device.write(CRLF);
        }
        priv->device.write(CRLF);

        priv->formattingState = Priv::MESSAGE_BODY;
    }
    case Priv::MESSAGE_BODY:
    {
        priv->device.write(QByteArray::number(chunk.size(), 16));
        priv->device.write(CRLF);
        priv->device.write(chunk);
        priv->device.write(CRLF);
    }
    } // switch (priv->formattingState)
    return true;
}

bool HttpServerResponse::addTrailers(const Headers &headers)
{
    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0))
        return false;

    switch (priv->formattingState) {
    case Priv::STATUS_LINE:
    case Priv::END:
    case Priv::HEADERS:
        return false;
    case Priv::MESSAGE_BODY:
        priv->device.write("0\r\n");
        priv->formattingState = Priv::TRAILERS;
    case Priv::TRAILERS:
    {
        for (Headers::const_iterator i = headers.constBegin()
             ;i != headers.end();++i) {
            priv->device.write(i.key());
            priv->device.write(": ", 2);
            priv->device.write(i.value());
            priv->device.write(CRLF);
        }
    }
    } // switch (priv->formattingState)
    return true;
}

bool HttpServerResponse::addTrailer(const QByteArray &headerName,
                                    const QByteArray &headerValue)
{
    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0))
        return false;

    switch (priv->formattingState) {
    case Priv::STATUS_LINE:
    case Priv::END:
    case Priv::HEADERS:
        return false;
    case Priv::MESSAGE_BODY:
        priv->device.write(LAST_CHUNK);
        priv->formattingState = Priv::TRAILERS;
    case Priv::TRAILERS:
    {
        priv->device.write(headerName);
        priv->device.write(": ", 2);
        priv->device.write(headerValue);
        priv->device.write(CRLF);
    }
    } // switch (priv->formattingState)
    return true;
}

bool HttpServerResponse::end(const QByteArray &chunk)
{
    switch (priv->formattingState) {
    case Priv::STATUS_LINE:
    case Priv::END:
        return false;
    case Priv::HEADERS:
    {
        bool continue_to_message_body
            = chunk.size() || priv->http10Buffer.size();

        if (priv->options.testFlag(HttpServerResponse::HTTP_1_1)) {
            static const char key[] = "Connection";
            if (priv->options.testFlag(HttpServerResponse::KEEP_ALIVE)) {
                static const char value[] = "keep-alive";
                priv->headers
                    .replace(QByteArray::fromRawData(key, sizeof(key) - 1),
                             QByteArray::fromRawData(value, sizeof(value) - 1));
            } else {
                static const char value[] = "close";
                priv->headers
                    .replace(QByteArray::fromRawData(key, sizeof(key) - 1),
                             QByteArray::fromRawData(value, sizeof(value) - 1));
            }
        }

        if (priv->options.testFlag(HttpServerResponse::HTTP_1_1)
            && continue_to_message_body) {
            static const char key[] = "Transfer-Encoding", value[] = "chunked";
            priv->headers
                .insert(QByteArray::fromRawData(key, sizeof(key) - 1),
                        QByteArray::fromRawData(value, sizeof(value) - 1));
        } else {
            static const char key[] = "Content-Length";
            priv->headers.replace(QByteArray
                                  ::fromRawData(key, sizeof(key) - 1),
                                  QByteArray::number(priv->http10Buffer.size()
                                                     + chunk.size()));
        }

        for (Headers::iterator i = priv->headers.begin()
             ;i != priv->headers.end();++i) {
            priv->device.write(i.key());
            priv->device.write(": ", 2);
            priv->device.write(i.value());
            priv->device.write(CRLF);
        }
        priv->device.write(CRLF);

        if (continue_to_message_body) {
            priv->formattingState = Priv::MESSAGE_BODY;
        } else {
            if (priv->options.testFlag(HttpServerResponse::HTTP_1_0)
                || !priv->options.testFlag(HttpServerResponse::KEEP_ALIVE)) {
                priv->device.close();
            }

            priv->formattingState = Priv::END;
            emit finished();
            return true;
        }
    }
    case Priv::MESSAGE_BODY:
    {
        if (chunk.size()) {
            if (priv->options.testFlag(HttpServerResponse::HTTP_1_1)) {
                priv->device.write(QByteArray::number(chunk.size(), 16));
                priv->device.write(CRLF);
            } else if (priv->http10Buffer.size()) {
                priv->device.write(priv->http10Buffer);
                priv->http10Buffer.clear();
            }
            priv->device.write(chunk);
            if (priv->options.testFlag(HttpServerResponse::HTTP_1_1))
                priv->device.write(CRLF);
        } else if (priv->http10Buffer.size()) {
            priv->device.write(priv->http10Buffer);
            priv->http10Buffer.clear();
        }
        if (priv->options.testFlag(HttpServerResponse::HTTP_1_1)) {
            priv->device.write(LAST_CHUNK);
            priv->formattingState = Priv::TRAILERS;
        } else {
            priv->device.close();
            priv->formattingState = Priv::END;
            emit finished();
            break;
        }
    }
    case Priv::TRAILERS:
    {
        priv->device.write(CRLF);
        if (!priv->options.testFlag(HttpServerResponse::KEEP_ALIVE))
            priv->device.close();

        priv->formattingState = Priv::END;
        emit finished();
    }
    } // switch (priv->formattingState)
    return true;
}

} // namespace Tufao
