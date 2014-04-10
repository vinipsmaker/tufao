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

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <algorithm>

namespace Tufao {

using MappingList = std::initializer_list<HttpServerRequestRouter::Mapping>;

HttpServerRequestRouter::HttpServerRequestRouter(QObject *parent) :
    QObject(parent),
    priv(new Priv)
{
}

HttpServerRequestRouter::HttpServerRequestRouter(MappingList mappings,
                                                 QObject *parent) :
    QObject(parent),
    priv(new Priv{mappings})
{
}

HttpServerRequestRouter::~HttpServerRequestRouter()
{
    delete priv;
}

int HttpServerRequestRouter::map(Mapping map)
{
    int i = priv->mappings.size();
    priv->mappings.push_back(map);
    return i;
}

int HttpServerRequestRouter::map(std::initializer_list<Mapping> map)
{
    int i = priv->mappings.size();
    std::copy(std::begin(map), std::end(map), std::end(priv->mappings));
    return i;
}

void HttpServerRequestRouter::unmap(int index)
{
    priv->mappings.remove(index);
}

void HttpServerRequestRouter::clear()
{
    priv->mappings.clear();
}

// TODO: Implement cache and use a tree to improve speed
bool HttpServerRequestRouter::handleRequest(HttpServerRequest &request,
                                            HttpServerResponse &response)
{
    const QString path{request.url().path()};

    for (const auto &mapping: priv->mappings) {
        QRegularExpressionMatch match{mapping.path.match(path)};

        if (match.hasMatch()) {
            if (mapping.method.size() && request.method() != mapping.method)
                continue;

            QStringList args{match.capturedTexts().mid(1)};
            QVariant backup{request.customData()};

            if (args.size()) {
                QVariantMap options{backup.toMap()};
                options["args"] = options["ags"].toStringList() + args;
                request.setCustomData(options);
            }

            if (mapping.handler(request, response))
                return true;

            if (args.size())
                request.setCustomData(backup);
        }
    }

    return false;
}

} // namespace Tufao
