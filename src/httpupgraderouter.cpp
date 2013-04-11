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

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "priv/httpupgraderouter.h"
#include "httpserverrequest.h"

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <algorithm>

namespace Tufao {

using MappingList = std::initializer_list<HttpUpgradeRouter::Mapping>;

HttpUpgradeRouter::HttpUpgradeRouter(QObject *parent) :
    QObject(parent),
    priv(new Priv)
{
}

HttpUpgradeRouter::HttpUpgradeRouter(MappingList mappings, QObject *parent) :
    QObject(parent),
    priv(new Priv{mappings})
{
}

HttpUpgradeRouter::~HttpUpgradeRouter()
{
    delete priv;
}

int HttpUpgradeRouter::map(Mapping map)
{
    int i = priv->mappings.size();
    priv->mappings.push_back(map);
    return i;
}

int HttpUpgradeRouter::map(std::initializer_list<Mapping> map)
{
    int i = priv->mappings.size();
    std::copy(std::begin(map), std::end(map), std::end(priv->mappings));
    return i;
}

void HttpUpgradeRouter::unmap(int index)
{
    priv->mappings.remove(index);
}

void HttpUpgradeRouter::clear()
{
    priv->mappings.clear();
}

bool HttpUpgradeRouter::handleUpgrade(HttpServerRequest &request,
                                      const QByteArray &head)
{
    const QString path{request.url().path()};

    for (const auto &mapping: priv->mappings) {
        QRegularExpressionMatch match{mapping.path.match(path)};

        if (match.hasMatch()) {
            QStringList args{match.capturedTexts().mid(1)};
            QVariant backup{request.customData()};

            if (args.size()) {
                QVariantMap options{backup.toMap()};
                options["args"] = options["ags"].toStringList() + args;
                request.setCustomData(options);
            }

            if (mapping.handler(request, head))
                return true;

            if (args.size())
                request.setCustomData(backup);
        }
    }

    return false;
}

} // namespace Tufao
