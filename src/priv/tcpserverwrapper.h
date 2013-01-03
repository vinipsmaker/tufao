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

namespace Tufao {

class TcpServerWrapper : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServerWrapper(QObject *parent = 0);

signals:
    void newConnection(qintptr socketDescriptor);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

} // namespace Tufao

#endif // TCPSERVERWRAPPER_H
