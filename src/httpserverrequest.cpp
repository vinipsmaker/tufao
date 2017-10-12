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
    priv(new Priv(this, socket))
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

    std::size_t nparsed = 0;
    boost::system::error_code ec;
    priv->whatEmit = Priv::Signals(0);
    priv->is_upgrade = false;

    do {
        std::array<asio::const_buffer, 1> buffers;
        buffers[0]
            = asio::buffer(priv->buffer.data(), priv->buffer.size()) + nparsed;
        nparsed += priv->put(buffers, ec);
    } while (!ec && !priv->whatEmit.testFlag(Priv::END));

    if (ec && ec != http::error::need_more) {
        priv->socket.close();
        return;
    }

    priv->buffer.remove(0, nparsed);

    if (priv->is_upgrade) {
        disconnect(&priv->socket, SIGNAL(readyRead()),
                   this, SLOT(onReadyRead()));
        disconnect(&priv->socket, SIGNAL(disconnected()),
                   this, SIGNAL(close()));
        disconnect(&priv->timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

        priv->body.swap(priv->buffer);
        emit upgrade();
        return;
    }

    Priv::Signals whatEmit(priv->whatEmit);

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
