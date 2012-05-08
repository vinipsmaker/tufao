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

#include "priv/httpserverrequestrouter.h"
#include "httpserverrequest.h"
#include "url.h"

#include <QtCore/QVector>
#include <QtCore/QStringList>

namespace Tufao {

typedef AbstractHttpServerRequestHandler Handler;

HttpServerRequestRouter::HttpServerRequestRouter(QObject *parent) :
    AbstractHttpServerRequestHandler(parent),
    priv(new Priv)
{
}

HttpServerRequestRouter::~HttpServerRequestRouter()
{
    delete priv;
}

HttpServerRequestRouter &HttpServerRequestRouter::map(const QRegExp &path,
                                                      Handler *handler)
{
    priv->general.push_back(QPair<QRegExp, Handler*>(path, handler));
    return *this;
}

HttpServerRequestRouter &HttpServerRequestRouter::map(const QRegExp &path,
                                                      const QByteArray &method,
                                                      Handler *handler)
{
    priv->methods[method].push_back(QPair<QRegExp, Handler*>(path, handler));
    return *this;
}

HttpServerRequestRouter &HttpServerRequestRouter::unmap(const QRegExp &path,
                                                        Handler *handler)
{
    priv->general.removeAll(QPair<QRegExp, Handler*>(path, handler));
    return *this;
}

HttpServerRequestRouter
&HttpServerRequestRouter::unmap(const QRegExp &path, const QByteArray &method,
                                Handler *handler)
{
    priv->methods[method].removeAll(QPair<QRegExp, Handler*>(path, handler));
    return *this;
}

HttpServerRequestRouter &HttpServerRequestRouter::unmap(const QRegExp &path)
{
    for (int i = priv->general.size() - 1;i >= 0;--i) {
        if (priv->general[i].first == path)
            priv->general.removeAt(i);
    }

    QList<QByteArray> methods(priv->methods.keys());
    foreach (const QByteArray &method, methods) {
        for (int i = priv->methods[method].size() - 1;i >= 0;--i) {
            if (priv->methods[method][i].first == path)
                priv->methods[method].removeAt(i);
        }
    }

    return *this;
}

HttpServerRequestRouter
&HttpServerRequestRouter::unmap(const QRegExp &path, const QByteArray &method)
{
    if (!priv->methods.contains(method))
        return *this;

    for (int i = priv->methods[method].size() - 1;i >= 0;--i) {
        if (priv->methods[method][i].first == path)
            priv->methods[method].removeAt(i);
    }

    return *this;
}

HttpServerRequestRouter &HttpServerRequestRouter::unmap(Handler *handler)
{
    for (int i = priv->general.size() - 1;i >= 0;--i) {
        if (priv->general[i].second == handler)
            priv->general.removeAt(i);
    }

    QList<QByteArray> methods(priv->methods.keys());
    foreach (const QByteArray &method, methods) {
        for (int i = priv->methods[method].size() - 1;i >= 0;--i) {
            if (priv->methods[method][i].second == handler)
                priv->methods[method].removeAt(i);
        }
    }

    return *this;
}

void HttpServerRequestRouter::clear()
{
    priv->general.clear();
    priv->methods.clear();
}

// TODO: Implement cache
bool HttpServerRequestRouter::handleRequest(HttpServerRequest *request,
                                            HttpServerResponse *response,
                                            const QStringList &args)
{
    QString path(QByteArray::fromPercentEncoding(Url(request->url())
                                                 .path().toUtf8()));

    if (priv->methods.contains(request->method())) {
        for (int i = 0;i != priv->methods[request->method()].size();++i) {
            QRegExp rx(priv->methods[request->method()][i].first);
            if (rx.indexIn(path) != -1) {
                if (priv->methods[request->method()][i].second->handleRequest
                        (request, response, args + rx.capturedTexts().mid(1))) {
                    return true;
                }
            }
        }
    }

    for (int i = 0;i != priv->general.size();++i) {
        QRegExp rx(priv->general[i].first);
        if (rx.indexIn(path) != -1) {
            if (priv->general[i].second->handleRequest(request, response, args
                                                       + rx.capturedTexts()
                                                       .mid(1))) {
                return true;
            }
        }
    }

    return false;
}

} // namespace Tufao
