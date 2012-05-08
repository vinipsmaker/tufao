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

#include "url.h"
#include "priv/url.h"

#include "httpserverrequest.h"
#include "headers.h"

#include <QtNetwork/QSslSocket>
#include <QtNetwork/QHostAddress>

// Groups:
// 1  = scheme
// 2  = authority
// 3  = userinfo
// 4  = username
// 5  = userpass
// 6  = hostname
// 7  = port
// 8  = path
// 9  = query
// 10 = fragment
static QRegExp regex("^(?:([^:/?#]+):)?"
                     "(?://((?:(([^/?#@:]*)(?::([^/?#@]*))?)@)?"
                     "([^/?#:]*)(?::([^/?#]*))?))?"
                     "([^?#]*)"
                     "(?:\\?([^#]*))?"
                     "(?:#(.*))?");

namespace Tufao {

Url::Url(const QString &url) :
    priv(new Priv(regex))
{
    priv->regex.indexIn(url);
}

Url::Url(const Url &url) :
    priv(new Priv(url.priv->regex))
{
}

Url::~Url()
{
    delete priv;
}

Url &Url::operator =(const Url &url)
{
    if (this == &url)
        return *this;

    delete priv;
    priv = new Priv(url.priv->regex);
    return *this;
}

QString Url::scheme() const
{
    return priv->regex.cap(1);
}

QString Url::authority() const
{
    return priv->regex.cap(2);
}

QString Url::path() const
{
    return priv->regex.cap(8);
}

QString Url::query() const
{
    return priv->regex.cap(9);
}

QString Url::fragment() const
{
    return priv->regex.cap(10);
}

QString Url::userinfo() const
{
    return priv->regex.cap(3);
}

QString Url::hostname() const
{
    return priv->regex.cap(6);
}

QString Url::port() const
{
    return priv->regex.cap(7);
}

QString Url::username() const
{
    return priv->regex.cap(4);
}

QString Url::password() const
{
    return priv->regex.cap(5);
}

QByteArray Url::url(HttpServerRequest *request)
{
    QByteArray host(request->headers().value("Host"));
    if (host.isEmpty())
        host = request->socket()->localAddress().toString().toUtf8();

    if (qobject_cast<QSslSocket*>(request->socket()))
        return "https://" + host + request->url();
    else
        return "http://" + host + request->url();
}

} // namespace Tufao
