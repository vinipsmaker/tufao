/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "priv/sessionstore.h"
#include "priv/cryptography.h"
#include "headers.h"

/*!
 * \def PAST
 * This macro expands to some QDateTime in the past;
 *
 * To remove a cookie, the server returns a Set-Cookie header with an expiration
 * date in the past.
 */
#define PAST QDateTime(QDate(2003, 12, 1))

namespace Tufao {

SessionStore::SessionStore(const SessionSettings &settings, QObject *parent) :
    QObject(parent),
    settings(settings),
    priv(new Priv)
{
}

SessionStore::~SessionStore()
{
    delete priv;
}

QByteArray SessionStore::session(const HttpServerRequest &request) const
{
    // init variables
    QList<QByteArray> headers(request.headers().values("Cookie"));

    // try to find a compatible cookie...
    // ...and returns the first
    for (int i = 0;i != headers.size();++i) {
        QList<QNetworkCookie> cookies(QNetworkCookie::parseCookies(headers[i]));

        for (int i = 0;i != cookies.size();++i) {
            if (cookies[i].name() == settings.name)
                return unsignSession(cookies[i].value());
        }
    }

    // cannot find a compatible cookie...
    // ...returns an empty value
    return QByteArray();
}

QByteArray SessionStore::session(const HttpServerRequest &request,
                                 const HttpServerResponse &response) const
{
    // init variables
    QList<QByteArray> headers(response.headers().values("Set-Cookie"));

    // try to find a compatible cookie...
    // ...and returns the first
    for (int i = 0;i != headers.size();++i) {
        QList<QNetworkCookie> cookies(QNetworkCookie::parseCookies(headers[i]));

        for (int i = 0;i != cookies.size();++i) {
            if (cookies[i].name() == settings.name)
                return unsignSession(cookies[i].value());
        }
    }

    // cannot find a compatible cookie in response's Set-Cookie header...
    // ...try request's "Cookie" header
    return session(request);
}

void SessionStore::setSession(HttpServerResponse &response,
                              const QByteArray &session) const
{
    response.headers()
        .insert("Set-Cookie",
                settings.cookie(signSession(session)).toRawForm());
}

void SessionStore::unsetSession(HttpServerResponse &response) const
{
    QNetworkCookie cookie(settings.cookie());
    cookie.setExpirationDate(PAST);
    response.headers().insert("Set-Cookie", cookie.toRawForm());
}

void SessionStore::resetSession(HttpServerRequest &request) const
{
    // init variables
    QList<QByteArray> headers(request.headers().values("Cookie"));
    QByteArray newValue;

    // remove old cookies
    request.headers().remove("Cookie");

    // find cookies that don't match this store's settings
    for (int i = 0;i != headers.size();++i) {
        QList<QNetworkCookie> cookies(QNetworkCookie::parseCookies(headers[i]));

        for (int i = 0;i != cookies.size();++i) {
            if (cookies[i].name() != settings.name) {
                newValue
                    += cookies[i].toRawForm(QNetworkCookie::NameAndValueOnly)
                    + "; ";
            }
        }
    }

    if (!newValue.isEmpty()) {
        // removes the final "; "
        newValue.remove(newValue.size() - 2, 2);
    }

    // update the request headers
    request.headers().insert("Cookie", newValue);
}

void SessionStore::setMacSecret(const QByteArray &secret)
{
    priv->macSecret = secret;
}

SessionSettings SessionStore::defaultSettings()
{
    SessionSettings settings;

    settings.timeout = 15;
    settings.httpOnly = true;
    settings.name = "SID";
    settings.path = "/";
    settings.secure = false;

    return settings;
}

inline QByteArray SessionStore::signSession(const QByteArray &message) const
{
    if (priv->macSecret.isEmpty())
        return message;

    return message + ':' + hmacSha1(priv->macSecret, message);
}

inline QByteArray SessionStore::unsignSession(const QByteArray &message) const
{
    if (priv->macSecret.isEmpty())
        return message;

    const int indexOfColon(message.lastIndexOf(':'));
    const QByteArray unhashedMessage(message.left(indexOfColon));
    const QByteArray hash(message.mid(indexOfColon + 1));

    if (hmacSha1(priv->macSecret, unhashedMessage) != hash)
        return QByteArray();

    return unhashedMessage;
}

} // namespace Tufao
