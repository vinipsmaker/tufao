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

#ifndef HTTPSERVERRESPONSE_H
#define HTTPSERVERRESPONSE_H

#include <QObject>
#include "tufao_global.h"

template <class Key, class T> class QMap;

namespace Tufao {

class TUFAOSHARED_EXPORT HttpServerResponse : public QObject
{
    Q_OBJECT
public:
    explicit HttpServerResponse(QObject *parent = 0);

    int statusCode() const;
    void setHeader(const QByteArray &name, const QByteArray &value);
    QByteArray header(const QByteArray &name);
    void removeHeader(const QByteArray &name);

public slots:
    void writeContinue();
    void writeHead(int statusCode, const QByteArray &reasonPhrase,
                   const QMap<QByteArray, QByteArray> &headers);
    void writeHead(int statusCode);
    void write(const QByteArray &chunk);

    void addTrailers(const QMap<QByteArray, QByteArray> &headers);
    void end(const QByteArray &chunk = QByteArray());
};

} // namespace Tufao

#endif // HTTPSERVERRESPONSE_H
