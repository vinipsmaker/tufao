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

struct HttpSettings
{
    HttpSettings();

    http_parser_settings settings;
};

inline HttpSettings::HttpSettings()
{
    settings.on_message_begin
            = Tufao::Priv::HttpServerRequest::on_message_begin;
    settings.on_url = Tufao::Priv::HttpServerRequest::on_url;
    settings.on_header_field = Tufao::Priv::HttpServerRequest::on_header_field;
    settings.on_header_value = Tufao::Priv::HttpServerRequest::on_header_value;
    settings.on_headers_complete
            = Tufao::Priv::HttpServerRequest::on_headers_complete;
    settings.on_body = Tufao::Priv::HttpServerRequest::on_body;
    settings.on_message_complete
            = Tufao::Priv::HttpServerRequest::on_message_complete;
}

static HttpSettings httpSettings;

namespace Tufao {

HttpServerRequest::HttpServerRequest(QAbstractSocket *socket, QObject *parent) :
    QObject(parent),
    priv(new Priv::HttpServerRequest(this, socket))
{
    if (!socket)
        return;

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SIGNAL(close()));
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

QAbstractSocket *HttpServerRequest::socket() const
{
    return priv->socket;
}

void HttpServerRequest::onReadyRead()
{
    priv->buffer.append(priv->socket->readAll());
    {
        int nparsed = http_parser_execute(&priv->parser,
                                          &httpSettings.settings,
                                          priv->buffer.data(),
                                          priv->buffer.size());
        priv->buffer.remove(0, nparsed);
    }

    if (priv->parser.http_errno) {
        priv->socket->close();
        return;
    }

    if (priv->parser.upgrade) {
        disconnect(priv->socket, SIGNAL(readyRead()),
                   this, SLOT(onReadyRead()));
        disconnect(priv->socket, SIGNAL(disconnected()), this, SIGNAL(close()));
        emit upgrade(priv->buffer);
        priv->buffer.clear();
    }
}

inline void HttpServerRequest::clear()
{
    priv->buffer.clear();
    priv->lastHeader.clear();
    priv->lastWasValue = true;
    priv->useTrailers = false;

    priv->method.clear();
    priv->url.clear();
    priv->httpVersion.clear();
    priv->headers.clear();
    priv->trailers.clear();
}

namespace Priv {

int HttpServerRequest::on_message_begin(http_parser *parser)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    request->clear();
    return 0;
}

int HttpServerRequest::on_url(http_parser *parser, const char *at,
                              size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    request->priv->url = QByteArray(at, length);
    return 0;
}

int HttpServerRequest::on_header_field(http_parser *parser, const char *at,
                                       size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    if (request->priv->lastWasValue) {
        request->priv->lastHeader = QByteArray(at, length);
        request->priv->lastWasValue = false;
    } else {
        request->priv->lastHeader.append(at, length);
    }
    return 0;
}

int HttpServerRequest::on_header_value(http_parser *parser, const char *at,
                                       size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    if (request->priv->lastWasValue) {
        if (request->priv->useTrailers)
            request->priv->trailers[request->priv->lastHeader]
                    .append(at, length);
        else
            request->priv->headers[request->priv->lastHeader]
                    .append(at, length);
    } else {
        if (request->priv->useTrailers) {
            if (request->priv->trailers.contains(request->priv->lastHeader))
                request->priv->trailers[request->priv->lastHeader].append(',');
            request->priv->trailers[request->priv->lastHeader]
                    = QByteArray(at, length);
        } else {
            if (request->priv->headers.contains(request->priv->lastHeader))
                request->priv->headers[request->priv->lastHeader].append(',');
            request->priv->headers[request->priv->lastHeader]
                    = QByteArray(at, length);
        }
        request->priv->lastWasValue = true;
    }
    return 0;
}

int HttpServerRequest::on_headers_complete(http_parser *parser)
{
    using Tufao::HttpServerResponse;
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);

    request->priv->lastHeader.clear();
    request->priv->lastWasValue = true;
    request->priv->useTrailers = true;

    request->priv->method
            = QByteArray(http_method_str(http_method(parser->method)));
    if (parser->http_minor == 1)
        request->priv->httpVersion = "HTTP/1.1";
    else
        request->priv->httpVersion = "HTTP/1.0";

    HttpServerResponse::Options options;

    if (parser->http_minor == 1)
        options |= HttpServerResponse::HTTP_1_1;
    else
        options |= HttpServerResponse::HTTP_1_0;

    if (request->priv->headers.is("Connection", "close"))
        options |= HttpServerResponse::CLOSE_CONNECTION;

    emit request->ready(options);

    return 0;
}

int HttpServerRequest::on_body(http_parser *parser, const char *at,
                               size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    emit request->data(QByteArray(at, length));
    return 0;
}

int HttpServerRequest::on_message_complete(http_parser *parser)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    emit request->end();
    return 0;
}

} // namespace Priv

} // namespace Tufao
