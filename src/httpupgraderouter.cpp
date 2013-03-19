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

#include "priv/httpupgraderouter.h"
#include "httpserverrequest.h"
#include "url.h"

namespace Tufao {

typedef AbstractHttpUpgradeHandler Handler;

HttpUpgradeRouter::HttpUpgradeRouter(QObject *parent) :
    AbstractHttpUpgradeHandler(parent),
    priv(new Priv)
{
}

HttpUpgradeRouter::~HttpUpgradeRouter()
{
    delete priv;
}

HttpUpgradeRouter &HttpUpgradeRouter::map(const QRegExp &path, Handler *handler)
{
    priv->general.push_back(QPair<QRegExp, Handler*>(path, handler));
    return *this;
}

HttpUpgradeRouter &HttpUpgradeRouter::unmap(const QRegExp &path,
                                            Handler *handler)
{
    priv->general.removeAll(QPair<QRegExp, Handler*>(path, handler));
    return *this;
}

HttpUpgradeRouter &HttpUpgradeRouter::unmap(const QRegExp &path)
{
    for (int i = priv->general.size() - 1;i >= 0;--i) {
        if (priv->general[i].first == path)
            priv->general.removeAt(i);
    }

    return *this;
}

HttpUpgradeRouter &HttpUpgradeRouter::unmap(Handler *handler)
{
    for (int i = priv->general.size() - 1;i >= 0;--i) {
        if (priv->general[i].second == handler)
            priv->general.removeAt(i);
    }

    return *this;
}

void HttpUpgradeRouter::clear()
{
    priv->general.clear();
}

// TODO: Implement cache
bool HttpUpgradeRouter::handleRequest(HttpServerRequest *request,
                                      const QByteArray &head,
                                      const QStringList &args)
{
    QString path(QByteArray::fromPercentEncoding(Url(request->url())
                                                 .path().toUtf8()));

    for (int i = 0;i != priv->general.size();++i) {
        QRegExp rx(priv->general[i].first);
        if (rx.indexIn(path) != -1) {
            if (priv->general[i].second->handleRequest(request, head, args
                                                       + rx.capturedTexts()
                                                       .mid(1))) {
                return true;
            }
        }
    }

    return false;
}

} // namespace Tufao
