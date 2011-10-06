/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HTTPSERVERREQUEST_H
#define HTTPSERVERREQUEST_H

#include <QObject>
#include "tufao_global.h"

class QAbstractSocket;
template <class Key, class T> class QMap;

namespace Tufao {

class TUFAOSHARED_EXPORT HttpServerRequest : public QObject
{
    Q_OBJECT
public:
    enum Error {
        ABORTED,
        TIMEOUT
    };

    explicit HttpServerRequest(QObject *parent = 0);

    QByteArray method() const;
    QByteArray url() const;
    QMap<QByteArray, QByteArray> headers() const;
    QMap<QByteArray, QByteArray> trailers() const;
    QByteArray httpVersion() const;

    QAbstractSocket *connection();

signals:
    void data(QByteArray data);
    void end();
    void close(Tufao::HttpServerRequest::Error error);
};

} // namespace Tufao

#endif // HTTPSERVERREQUEST_H
