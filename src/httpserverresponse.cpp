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

#include "httpserverresponse.h"
#include "priv/httpserverresponse.h"
#include "priv/reasonphrase.h"

namespace Tufao {

HttpServerResponse::HttpServerResponse(QIODevice *device, Options options,
                                       QObject *parent) :
    QObject(parent),
    priv(new Priv::HttpServerResponse(device, options))
{
}

HttpServerResponse::~HttpServerResponse()
{
    delete priv;
}

Headers &HttpServerResponse::headers()
{
    return priv->headers;
}

bool HttpServerResponse::writeContinue()
{
    if (priv->formattingState != Priv::STATUS_LINE
            || !priv->device)
        return false;

    if (priv->options.testFlag(HTTP_1_1))
        priv->device->write("HTTP/1.1 100 Continue\r\n\r\n");
    else
        priv->device->write("HTTP/1.0 100 Continue\r\n\r\n");
    return true;
}

bool HttpServerResponse::writeHead(int statusCode,
                                   const QByteArray &reasonPhrase,
                                   const Headers &headers)
{
    if (priv->formattingState != Priv::STATUS_LINE
            || !priv->device)
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0))
        priv->device->write("HTTP/1.0 ");
    else
        priv->device->write("HTTP/1.1 ");

    priv->device->write(QByteArray::number(statusCode));
    priv->device->write(" ");
    priv->device->write(reasonPhrase);
    priv->device->write("\r\n");

    for (Headers::const_iterator i = headers.constBegin()
         ;i != headers.end();++i) {
        priv->device->write(i.key());
        priv->device->write(": ");
        priv->device->write(i.value());
        priv->device->write("\r\n");
    }
    priv->formattingState = Priv::HEADERS;
    return true;
}

bool HttpServerResponse::writeHead(int statusCode, const Headers &headers)
{
    if (priv->formattingState != Priv::STATUS_LINE
            || !priv->device)
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0))
        priv->device->write("HTTP/1.0 ");
    else
        priv->device->write("HTTP/1.1 ");

    priv->device->write(QByteArray::number(statusCode));
    priv->device->write(" ");
    priv->device->write(Priv::reasonPhrase(statusCode));
    priv->device->write("\r\n");

    for (Headers::const_iterator i = headers.constBegin()
         ;i != headers.end();++i) {
        priv->device->write(i.key());
        priv->device->write(": ");
        priv->device->write(i.value());
        priv->device->write("\r\n");
    }
    priv->formattingState = Priv::HEADERS;
    return true;
}

bool HttpServerResponse::writeHead(int statusCode)
{
    if (priv->formattingState != Priv::STATUS_LINE
            || !priv->device)
        return false;

    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0))
        priv->device->write("HTTP/1.0 ");
    else
        priv->device->write("HTTP/1.1 ");

    priv->device->write(QByteArray::number(statusCode));
    priv->device->write(" ");
    priv->device->write(Priv::reasonPhrase(statusCode));
    priv->device->write("\r\n");
    priv->formattingState = Priv::HEADERS;
    return true;
}

bool HttpServerResponse::write(const QByteArray &chunk)
{
    if (priv->options.testFlag(HttpServerResponse::HTTP_1_0)
            || !chunk.size()) {
        return false;
    }

    switch (priv->formattingState) {
    case Priv::STATUS_LINE:
    case Priv::END:
    case Priv::TRAILERS:
        return false;
    case Priv::HEADERS:
    {
        if (priv->options.testFlag(HttpServerResponse::KEEP_ALIVE))
            priv->headers.replace("Connection", "keep-alive");
        else
            priv->headers.replace("Connection", "close");
        priv->headers.insert("Transfer-Encoding", "chunked");

        for (Headers::iterator i = priv->headers.begin()
             ;i != priv->headers.end();++i) {
            priv->device->write(i.key());
            priv->device->write(": ");
            priv->device->write(i.value());
            priv->device->write("\r\n");
        }
        priv->device->write("\r\n");

        priv->formattingState = Priv::MESSAGE_BODY;
    }
    case Priv::MESSAGE_BODY:
    {
        priv->device->write(QByteArray::number(chunk.size(), 16));
        priv->device->write("\r\n");
        priv->device->write(chunk);
        priv->device->write("\r\n");
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
        priv->device->write("0\r\n");
        priv->formattingState = Priv::TRAILERS;
    case Priv::TRAILERS:
    {
        for (Headers::const_iterator i = headers.constBegin()
             ;i != headers.end();++i) {
            priv->device->write(i.key());
            priv->device->write(": ");
            priv->device->write(i.value());
            priv->device->write("\r\n");
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
        priv->device->write("0\r\n");
        priv->formattingState = Priv::TRAILERS;
    case Priv::TRAILERS:
    {
        priv->device->write(headerName);
        priv->device->write(": ");
        priv->device->write(headerValue);
        priv->device->write("\r\n");
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
        if (priv->options.testFlag(HttpServerResponse::HTTP_1_1)) {
            if (priv->options.testFlag(HttpServerResponse::KEEP_ALIVE))
                priv->headers.replace("Connection", "keep-alive");
            else
                priv->headers.replace("Connection", "close");
            priv->headers.insert("Transfer-Encoding", "chunked");
        } else {
            priv->headers.replace("Content-Length",
                                  QByteArray::number(chunk.size()));
        }

        for (Headers::iterator i = priv->headers.begin()
             ;i != priv->headers.end();++i) {
            priv->device->write(i.key());
            priv->device->write(": ");
            priv->device->write(i.value());
            priv->device->write("\r\n");
        }
        priv->device->write("\r\n");

        priv->formattingState = Priv::MESSAGE_BODY;
    }
    case Priv::MESSAGE_BODY:
    {
        if (chunk.size()) {
            if (priv->options.testFlag(HttpServerResponse::HTTP_1_1)) {
                priv->device->write(QByteArray::number(chunk.size(), 16));
                priv->device->write("\r\n");
            }
            priv->device->write(chunk);
            if (priv->options.testFlag(HttpServerResponse::HTTP_1_1))
                priv->device->write("\r\n");
        }
        if (priv->options.testFlag(HttpServerResponse::HTTP_1_1)) {
            priv->device->write("0\r\n");
            priv->formattingState = Priv::TRAILERS;
        } else {
            priv->device->close();
            priv->formattingState = Priv::END;
            emit finished();
            break;
        }
    }
    case Priv::TRAILERS:
    {
        priv->device->write("\r\n");
        if (!priv->options.testFlag(HttpServerResponse::KEEP_ALIVE))
            priv->device->close();

        priv->formattingState = Priv::END;
        emit finished();
    }
    } // switch (priv->formattingState)
    return true;
}

} // namespace Tufao
