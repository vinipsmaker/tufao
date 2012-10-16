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

#ifndef TCPSERVERWRAPPER_H
#define TCPSERVERWRAPPER_H

#include <QtNetwork/QTcpServer>
#include "../tufao_global.h"

// moc isn't properly including "../tufao_global.h", so I need to provide the
// below definition manually:
// ==== BEGIN ====

#ifdef Q_MOC_RUN
#define QT_VERSION_CHECK(major,minor,patch) ((major<<16)|(minor<<8)|(patch))
// QT_VERSION_* macros should be provided through -D when you call moc
#define QT_VERSION \
    QT_VERSION_CHECK(QT_VERSION_MAJOR,QT_VERSION_MINOR,QT_VERSION_PATCH)
#endif // Q_MOC_RUN

// ====  END  ====

namespace Tufao {

class TcpServerWrapper : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServerWrapper(QObject *parent = 0);

signals:
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    void newConnection(qintptr socketDescriptor);
#else
    void newConnection(int socketDescriptor);
#endif

protected:
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    void incomingConnection(qintptr socketDescriptor);
#else
    void incomingConnection(int socketDescriptor);
#endif
};

} // namespace Tufao

#endif // TCPSERVERWRAPPER_H
