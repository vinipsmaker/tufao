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

#include "abstractmessagesocket.h"

namespace Tufao {

AbstractMessageSocket::AbstractMessageSocket(QObject *parent) :
    QObject(parent),
    connected_(false)
{
    connect(this, &AbstractMessageSocket::connected,
            this, &AbstractMessageSocket::onConnected);
    connect(this, &AbstractMessageSocket::disconnected,
            this, &AbstractMessageSocket::onDisconnected);
}

bool AbstractMessageSocket::isConnected() const
{
    return connected_;
}

void AbstractMessageSocket::onConnected()
{
    connected_ = true;
}

void AbstractMessageSocket::onDisconnected()
{
    connected_ = false;
}

} // namespace Tufao
