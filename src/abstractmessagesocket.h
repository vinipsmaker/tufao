/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#ifndef TUFAO_ABSTRACTMESSAGEDISPATCHER_H
#define TUFAO_ABSTRACTMESSAGEDISPATCHER_H

#include <QtCore/QObject>

namespace Tufao {

class AbstractMessageSocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected FINAL)
public:
    explicit AbstractMessageSocket(QObject *parent = 0);

    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void newMessage(QByteArray msg);

public slots:
    virtual void close() = 0;
    virtual bool sendMessage(const QByteArray &msg) = 0;

private slots:
    void onConnected();
    void onDisconnected();

private:
    bool _connected;
};

} // namespace Tufao

#endif // TUFAO_ABSTRACTMESSAGEDISPATCHER_H
