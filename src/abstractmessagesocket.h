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
#include "tufao_global.h"

namespace Tufao {

/*!
  The Tufao::AbstractMessageSocket class represents a socket that sends and
  receives messages.

  Classes implementing Tufao::AbstractMessageSocket can provide several high
  level functionalities, such as:
    - Messages separation and interleaving
    - Messages compression
    - Authentication
    - Proxy or tunnel
    - ...

  It's main purpose is to allow easily change the message exchange mechanism in
  algorithms that depends on message passing. You could use it, for example, to
  allow a class that provide a RPC mechanism to work on different connections
  types.

  \since 0.2
  */
class TUFAO_EXPORT AbstractMessageSocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected FINAL)
public:
    /*!
      Constructs a Tufao::AbstractMessageSocket object.

      \p parent is passed to the QObject constructor.
      */
    explicit AbstractMessageSocket(QObject *parent = 0);

    /*!
      Returns true if the connection is open.
      */
    bool isConnected() const;

signals:
    /*!
      This signal should be emitted when the connection is open.
      */
    void connected();

    /*!
      This signal should be emitted when the connection is closed or when fails
      to connect.
      */
    void disconnected();

    /*!
      This signal should be emitted each time a new message is available.
      */
    void newMessage(QByteArray msg);

public slots:
    /*!
      This method should close the connection.
      */
    virtual void close() = 0;

    /*!
      This method should send a new message if the connection is open.

      The object should discard the message if the connection is closed, but it
      may implement a different behavior (and return true).

      \return true if successful
      */
    virtual bool sendMessage(const QByteArray &msg) = 0;

private slots:
    void onConnected();
    void onDisconnected();

private:
    bool _connected;
};

} // namespace Tufao

#endif // TUFAO_ABSTRACTMESSAGEDISPATCHER_H
