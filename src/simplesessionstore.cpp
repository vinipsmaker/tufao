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

#include "priv/simplesessionstore.h"
#include <QtCore/QUuid>

namespace Tufao {

SimpleSessionStore::SimpleSessionStore(const SessionSettings &settings,
                                       QObject *parent) :
    SessionStore(settings, parent),
    priv(new Priv)
{
    priv->timer.setInterval(DEFAULT_REFRESH_INTERVAL);
    connect(&priv->timer, &QTimer::timeout,
            this, &SimpleSessionStore::onTimer);
    priv->timer.start();
}

SimpleSessionStore::~SimpleSessionStore()
{
    delete priv;
}

int SimpleSessionStore::refreshInterval() const
{
    return priv->timer.interval();
}

void SimpleSessionStore::setRefreshInterval(int msecs)
{
    priv->timer.setInterval(msecs);
}

bool SimpleSessionStore::hasSession(const HttpServerRequest &request) const
{
    QByteArray session(SessionStore::session(request));

    if (session.isEmpty() || !priv->lifetimeDatabase.contains(session))
        return false;

    // expired session...
    if (priv->lifetimeDatabase[session] <= QDateTime::currentDateTimeUtc()) {
        priv->lifetimeDatabase.remove(session);
        priv->database.remove(session);
        return false;
    }

    return true;
}

void SimpleSessionStore::removeSession(const HttpServerRequest &request,
                                       HttpServerResponse &response)
{
    QByteArray session(SessionStore::session(request, response));

    if (session.isEmpty())
        return;

    unsetSession(response);

    if (!priv->database.contains(session))
        return;

    priv->database.remove(session);
    priv->lifetimeDatabase.remove(session);
}

QList<QByteArray>
SimpleSessionStore::properties(const HttpServerRequest &request,
                               const HttpServerResponse &response) const
{
    QByteArray session(SessionStore::session(request, response));

    if (session.isEmpty() || !priv->lifetimeDatabase.contains(session))
        return QList<QByteArray>();

    // expired session...
    if (priv->lifetimeDatabase[session] <= QDateTime::currentDateTimeUtc()) {
        priv->lifetimeDatabase.remove(session);
        priv->database.remove(session);
        return QList<QByteArray>();
    }

    QList<QString> keys(priv->database[session].keys());
    QList<QByteArray> ret;

    ret.reserve(keys.size());

    for (int i = 0;i != keys.size();++i)
        ret += keys[i].toUtf8();

    return ret;
}

bool SimpleSessionStore::hasProperty(const HttpServerRequest &request,
                                     const HttpServerResponse &response,
                                     const QByteArray &key) const
{
    QByteArray session(SessionStore::session(request, response));

    if (session.isEmpty() || !priv->lifetimeDatabase.contains(session))
        return false;

    // expired session...
    if (priv->lifetimeDatabase[session] <= QDateTime::currentDateTimeUtc()) {
        priv->lifetimeDatabase.remove(session);
        priv->database.remove(session);
        return false;
    }

    return priv->database[session].contains(key);
}

QVariant SimpleSessionStore::property(const HttpServerRequest &request,
                                      HttpServerResponse &response,
                                      const QByteArray &key) const
{
    QByteArray session(SessionStore::session(request, response));

    if (session.isEmpty())
        return QVariant();

    if (!priv->lifetimeDatabase.contains(session)) {
        // possibly avoid useless future queries
        unsetSession(response);

        return QVariant();
    }

    // expired session...
    if (priv->lifetimeDatabase[session] <= QDateTime::currentDateTimeUtc()) {
        priv->lifetimeDatabase.remove(session);
        priv->database.remove(session);
        return QVariant();
    }

    // change session expire time
    priv->lifetimeDatabase[session]
        = QDateTime::currentDateTimeUtc().addSecs(settings.timeout * 60);

    // update cookie (expire time)
    setSession(response, session);

    const QVariantMap &sessionData(priv->database[session]);

    if (!sessionData.contains(key))
        return QVariant();

    return sessionData[key];
}

void SimpleSessionStore::setProperty(const HttpServerRequest &request,
                                     HttpServerResponse &response,
                                     const QByteArray &key,
                                     const QVariant &value)
{
    // init session variable
    QByteArray session(SessionStore::session(request, response));

    if (session.isEmpty() || !priv->database.contains(session))
        session = createSession();

    // set property
    priv->database[session][key] = value;

    // change session expire time
    priv->lifetimeDatabase[session]
        = QDateTime::currentDateTimeUtc().addSecs(settings.timeout * 60);

    // create, if not set yet, and update cookie (expire time)
    setSession(response, session);
}

void SimpleSessionStore::removeProperty(const HttpServerRequest &request,
                                        HttpServerResponse &response,
                                        const QByteArray &key)
{
    // init session variable
    QByteArray session(SessionStore::session(request, response));

    if (session.isEmpty() || !priv->database.contains(session))
        return;

    // remove property
    priv->database[session].remove(key);

    // change session expire time
    priv->lifetimeDatabase[session]
        = QDateTime::currentDateTimeUtc().addSecs(settings.timeout * 60);

    // update cookie (expire time)
    setSession(response, session);
}

SimpleSessionStore &SimpleSessionStore::defaultInstance()
{
    static SimpleSessionStore instance;
    return instance;
}

void SimpleSessionStore::onTimer()
{
    QHash<QByteArray, QDateTime>::iterator i(priv->lifetimeDatabase.begin());
    while (i != priv->lifetimeDatabase.end()) {
        if (i.value() <= QDateTime::currentDateTimeUtc()) {
            priv->database.remove(i.key());
            i = priv->lifetimeDatabase.erase(i);
            continue;
        }

        ++i;
    }
}

inline QByteArray SimpleSessionStore::createSession() const
{
    return QUuid::createUuid().toByteArray();
}

} // namespace Tufao
