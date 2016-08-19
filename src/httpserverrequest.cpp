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

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "priv/httpserverrequest.h"

namespace Tufao {

HttpServerRequest::HttpServerRequest(QAbstractSocket &socket, QObject *parent) :
    QObject(parent),
    priv(new Priv(socket))
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&socket, SIGNAL(disconnected()), this, SIGNAL(close()));

    connect(&priv->timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    if (priv->timeout)
        priv->timer.start(priv->timeout);
}

HttpServerRequest::~HttpServerRequest()
{
    delete priv;
}

QByteArray HttpServerRequest::method() const
{
    return priv->method;
}

void HttpServerRequest::setUrl(const QUrl &url)
{
    priv->url = url;
}

QUrl HttpServerRequest::url() const
{
    return priv->url;
}

Headers HttpServerRequest::headers() const
{
    return priv->headers;
}

Headers &HttpServerRequest::headers()
{
    return priv->headers;
}

Headers HttpServerRequest::trailers() const
{
    return priv->trailers;
}

HttpVersion HttpServerRequest::httpVersion() const
{
    return priv->httpVersion;
}

QByteArray HttpServerRequest::readBody()
{
    QByteArray body;
    body.swap(priv->body);
    return body;
}

QAbstractSocket &HttpServerRequest::socket() const
{
    return priv->socket;
}

void HttpServerRequest::setTimeout(int msecs)
{
    priv->timeout = msecs;

    if (priv->timeout)
        priv->timer.start(priv->timeout);
    else
        priv->timer.stop();
}

int HttpServerRequest::timeout() const
{
    return priv->timeout;
}

HttpServerResponse::Options HttpServerRequest::responseOptions() const
{
    return priv->responseOptions;
}

QVariant HttpServerRequest::customData() const
{
    return priv->customData;
}

void HttpServerRequest::setCustomData(const QVariant &data)
{
    priv->customData = data;
}

void HttpServerRequest::resume()
{
    connect(&priv->socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    if (priv->buffer.size())
        onReadyRead();
}

void HttpServerRequest::onReadyRead()
{
    if (priv->timeout)
        priv->timer.start(priv->timeout);

    priv->buffer += priv->socket.readAll();
    priv->parser.set_buffer(asio::buffer(priv->buffer.data(),
                                         priv->buffer.size()));

    std::size_t nparsed = 0;
    Priv::Signals whatEmit(0);
    bool is_upgrade = false;

    do {
        priv->parser.next();
        switch(priv->parser.code()) {
        case http::token::code::error_insufficient_data:
            continue;
        case http::token::code::error_set_method:
            qFatal("unreachable");
            break;
        case http::token::code::error_use_another_connection:
            qFatal("unreachable");
            break;
        case http::token::code::error_invalid_data:
        case http::token::code::error_no_host:
        case http::token::code::error_invalid_content_length:
        case http::token::code::error_content_length_overflow:
        case http::token::code::error_invalid_transfer_encoding:
        case http::token::code::error_chunk_size_overflow:
            priv->socket.close();
            return;
        case http::token::code::skip:
            break;
        case http::token::code::method:
            {
                clearRequest();
                priv->responseOptions = 0;
                auto value = priv->parser.value<http::token::method>();
                QByteArray method(value.data(), value.size());
                priv->method = std::move(method);
            }
            break;
        case http::token::code::request_target:
            {
                auto value = priv->parser.value<http::token::request_target>();
                QByteArray url(value.data(), value.size());
                priv->url = std::move(url);
            }
            break;
        case http::token::code::version:
            {
                auto value = priv->parser.value<http::token::version>();
                if (value == 0) {
                    priv->httpVersion = HttpVersion::HTTP_1_0;
                    priv->responseOptions |= HttpServerResponse::HTTP_1_0;
                } else {
                    priv->httpVersion = HttpVersion::HTTP_1_1;
                    priv->responseOptions |= HttpServerResponse::HTTP_1_1;
                }
            }
            break;
        case http::token::code::status_code:
            qFatal("unreachable");
            break;
        case http::token::code::reason_phrase:
            qFatal("unreachable");
            break;
        case http::token::code::field_name:
            {
                auto value = priv->parser.value<http::token::field_name>();
                priv->lastHeader = QByteArray(value.data(), value.size());
            }
            break;
        case http::token::code::field_value:
            {
                auto value = priv->parser.value<http::token::field_value>();
                QByteArray header(value.data(), value.size());
                (priv->useTrailers ? priv->trailers : priv->headers)
                    .insert(priv->lastHeader, std::move(header));
                priv->lastHeader.clear();
            }
            break;
        case http::token::code::end_of_headers:
            {
                auto it = priv->headers.find("connection");
                bool close_found = false;
                bool keep_alive_found = false;
                for (;it != priv->headers.end();++it) {
                    auto value = boost::string_ref(it->data(), it->size());
                    http::header_value_any_of(value, [&](boost::string_ref v) {
                        if (iequals(v, "close"))
                            close_found = true;

                        if (iequals(v, "keep-alive"))
                            keep_alive_found = true;

                        if (iequals(v, "upgrade"))
                            is_upgrade = true;

                        return false;
                    });
                    if (close_found)
                        break;
                }
                if (!close_found
                    && (priv->httpVersion == HttpVersion::HTTP_1_1
                        || keep_alive_found)) {
                    priv->responseOptions |= HttpServerResponse::KEEP_ALIVE;
                }
                whatEmit = Priv::READY;
            }
            break;
        case http::token::code::body_chunk:
            {
                auto value = priv->parser.value<http::token::body_chunk>();
                priv->body.append(asio::buffer_cast<const char*>(value),
                                  asio::buffer_size(value));
                whatEmit |= Priv::DATA;
            }
            break;
        case http::token::code::end_of_body:
            priv->useTrailers = true;
            break;
        case http::token::code::end_of_message:
            priv->useTrailers = false;
            priv->parser.set_buffer(asio::buffer(priv->buffer.data() + nparsed,
                                                 priv->parser.token_size()));
            whatEmit |= Priv::END;
            disconnect(&priv->socket, SIGNAL(readyRead()),
                       this, SLOT(onReadyRead()));
            break;
        }

        nparsed += priv->parser.token_size();
    } while(priv->parser.code() != http::token::code::end_of_message);
    priv->buffer.remove(0, nparsed);

    if (is_upgrade) {
        disconnect(&priv->socket, SIGNAL(readyRead()),
                   this, SLOT(onReadyRead()));
        disconnect(&priv->socket, SIGNAL(disconnected()),
                   this, SIGNAL(close()));
        disconnect(&priv->timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

        priv->body.swap(priv->buffer);
        emit upgrade();
        return;
    }

    if (whatEmit.testFlag(Priv::READY)) {
        whatEmit &= ~Priv::Signals(Priv::READY);
        this->disconnect(SIGNAL(data()));
        this->disconnect(SIGNAL(end()));
        emit ready();
    }

    if (whatEmit.testFlag(Priv::DATA)) {
        whatEmit &= ~Priv::Signals(Priv::DATA);
        emit data();
    }

    if (whatEmit.testFlag(Priv::END)) {
        whatEmit &= ~Priv::Signals(Priv::END);
        emit end();
        return;
    }
}

void HttpServerRequest::onTimeout()
{
    priv->socket.close();
}

inline void HttpServerRequest::clearRequest()
{
    priv->method.clear();
    priv->url.clear();
    priv->headers.clear();
    priv->body.clear();
    priv->trailers.clear();
    priv->customData.clear();
}

} // namespace Tufao
