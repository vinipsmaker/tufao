/*  This file is part of the Tufão project
    Copyright (C) 2013 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "priv/urlrewriterhandler.h"
#include "httpserverrequest.h"

namespace Tufao {

UrlRewriterHandler::UrlRewriterHandler(const QUrl &url, QObject *parent) :
    QObject(parent),
    priv(new Priv(url))
{
}

void UrlRewriterHandler::setUrl(const QUrl &url)
{
    priv->url = url;
}

QUrl UrlRewriterHandler::url() const
{
    return priv->url;
}

std::function<bool(HttpServerRequest&, HttpServerResponse&)>
UrlRewriterHandler::handler(const QUrl &url)
{
    return [url](HttpServerRequest &request, HttpServerResponse&) {
        request.setUrl(url);
        return false;
    };
}

bool UrlRewriterHandler::handleRequest(HttpServerRequest &request,
                                       HttpServerResponse &)
{
    request.setUrl(priv->url);
    return false;
}

} // namespace Tufao
