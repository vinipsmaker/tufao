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

#include "httpserverrequest.h"
#include "priv/httpserverrequest.h"
#include "priv/bufferwrapper.h"

// TODO: HttpDefines is the ugliest thing in the whole framework!
struct HttpDefines
{
    HttpDefines()
    {
        supportedMethods << "HEAD" << "GET" << "POST" << "PUT" << "DELETE"
                         << "TRACE" << "OPTIONS" << "CONNECT" << "PATCH";
        supportedHttpVersions << "HTTP/1.0" << "HTTP/1.1";
    }

    QList<QByteArray> supportedMethods;
    QList<QByteArray> supportedHttpVersions;
    QList<QByteArray> entityBody;
};

static HttpDefines httpDefines;

namespace Tufao {

HttpServerRequest::HttpServerRequest(QAbstractSocket *socket, QObject *parent) :
    QObject(parent),
    priv(new Priv::HttpServerRequest(socket))
{
    if (!socket)
        return;

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
//    connect(socket, SIGNAL(disconnected()), this, SIGNAL(close()));
}

HttpServerRequest::~HttpServerRequest()
{
    delete priv;
}

QByteArray HttpServerRequest::method() const
{
    return priv->method;
}

QByteArray HttpServerRequest::url() const
{
    return priv->url;
}

QMap<QByteArray, QByteArray> HttpServerRequest::headers() const
{
    return priv->headers.map;
}

QMap<QByteArray, QByteArray> HttpServerRequest::trailers() const
{
    return priv->trailers.map;
}

QByteArray HttpServerRequest::httpVersion() const
{
    return priv->httpVersion;
}

QAbstractSocket *HttpServerRequest::connection() const
{
    return priv->socket;
}

void HttpServerRequest::onReadyRead()
{
    priv->buffer.append(priv->socket->readAll());
    Priv::BufferWrapper bufferWrapper(priv->buffer);

    switch (priv->parsingState) {
    case Priv::READING_REQUEST_LINE:
    {
        // Certain buggy HTTP/1.0 client implementations generate extra CRLF’s
        // after a POST request.
        while (priv->buffer.startsWith("\r\n"))
            priv->buffer.remove(0, 2);

        if (!bufferWrapper.line())
            break;

        clear();

        QByteArray line = bufferWrapper.takeLine();

        QList<QByteArray> firstLine = line.simplified().split(' ');
        if (firstLine.size() != 3) {
            priv->socket->close();
            return;
        }

        if (!httpDefines.supportedMethods.contains(firstLine[0])
                || !httpDefines.supportedHttpVersions.contains(firstLine[2])) {
            priv->socket->close();
            return;
        }

        // HTTP request line:
        // <METHOD> <URL> <VERSION>
        priv->method = firstLine[0];
        priv->url = firstLine[1];
        priv->httpVersion = firstLine[2];

        priv->parsingState = Priv::READING_HEADERS;
    }
    case Priv::READING_HEADERS:
    {
        while (bufferWrapper.line()) {
            QByteArray line = bufferWrapper.takeLine();
            if (line.isEmpty()) {
                priv->parsingState = Priv::ANALYZYING_REQUEST;
                break;
            }

            QList<QByteArray> header = line.split(':');
            header.reserve(2);
            {
                const int indexOfColon = line.indexOf(':');
                header.push_back(line.left(indexOfColon));
                header.push_back(line.mid(indexOfColon + 1));
            }

            const QByteArray name = header[0].simplified();
            QByteArray value = header[1].simplified();

            if (priv->headers.contains(name)) {
                value.reserve(value.size() + 1 + priv->headers[name].size());
                value.append(',');
                value.append(priv->headers[name]);
            }

            priv->headers[name] = value;
        }
        if (priv->parsingState == Priv::READING_HEADERS)
            break;
    }
    case Priv::ANALYZYING_REQUEST:
    {
        HttpServerResponse::Options options;

        if (priv->httpVersion == "HTTP/1.0")
            options |= HttpServerResponse::HTTP_1_0;
        else
            options |= HttpServerResponse::HTTP_1_1;

        if (priv->headers.is("Connection", "close"))
            options |= HttpServerResponse::CLOSE_CONNECTION;

        // The presence of a message-body in a request is signaled by the
        // inclusion of a Content-Length or Transfer-Encoding header field in
        // the request’s message-headers.
        if (priv->headers.contains("Content-Length")) {
            priv->parsingState = Priv::READING_MESSAGE_BODY;

            bool ok;
            priv->messageLength = priv->headers["Content-Length"].toInt(&ok);
            if (!ok) {
                priv->socket->close();
                emit close();
                return;
            }

            // "Transfer-Encoding" accepts a comma separated list of case
            // insensitive values, with optional linear whitespace. The allowed
            // values are:
            // * chunked
            // * identity (no transformation)
            // * gzip
            // * compress
            // * deflate
            //
            // Whenever a transfer-coding is applied to a message-body, the set
            // of transfer-codings MUST include "chunked", unless the message is
            // terminated by closing the connection. When the "chunked"
            // transfer-coding is used, it MUST be the last transfer-coding
            // applied to the message-body. The "chunked" transfer-coding MUST
            // NOT be applied more than once to a message-body.
            //
            // TODO: currently the only handled transfer coding is chunked.
        } else if (priv->headers.contains("Transfer-Encoding")
                   && (!priv->headers.is("Connection", "close")
                       // The fastest way to check for a value is a simple
                       // substring match.
                       || priv->headers["Transfer-Encoding"]
                       .contains("chunked"))) {
            // TODO: implement chunked transfer-coding support
            qDebug("chunked entity not supported");
            priv->socket->close();
            emit close();
            return;
        } else {
            priv->parsingState = Priv::READING_REQUEST_LINE;
            emit ready(options);
            emit end();
            break;
        }

        emit ready(options);

        if (priv->parsingState != Priv::READING_MESSAGE_BODY)
            break;
    }
    case Priv::READING_MESSAGE_BODY:
    {
        switch (priv->messageBodyTransformations) {
        case Priv::IDENTITY:
            int bufferSize = priv->buffer.size();
            if (bufferSize) {
                emit data(priv->buffer.left(priv->messageLength));
                priv->buffer.remove(0, priv->messageLength);

                if (bufferSize >= priv->messageLength) {
                    priv->parsingState = Priv::READING_REQUEST_LINE;
                    emit end();
                } else {
                    priv->messageLength -= bufferSize;
                }
            }
        }
    }
    } // switch (priv->parsingState)
}

inline void HttpServerRequest::clear()
{
    priv->parsingState = Priv::READING_REQUEST_LINE;
    priv->method.clear();
    priv->url.clear();
    priv->httpVersion.clear();
    priv->headers.clear();
    priv->trailers.clear();
    priv->messageBodyTransformations = Priv::IDENTITY;
    priv->messageLength = 0;
}

} // namespace Tufao
