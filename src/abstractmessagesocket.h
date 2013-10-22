/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
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

      \note
      __This signal might be unsafe__ (read this: \ref safe-signal)!\n\n

      Unless you know what subclass of AbstractMessageSocket is being used
      __AND__ this subclass explicitly documents the opposite (signal is safe),
      you should assume that this signal is unsafe.
      */
    void connected();

    /*!
      This signal should be emitted when the connection is closed or when fails
      to connect.

      \note
      __This signal might be unsafe__ (read this: \ref safe-signal)!\n\n

      Unless you know what subclass of AbstractMessageSocket is being used
      __AND__ this subclass explicitly documents the opposite (signal is safe),
      you should assume that this signal is unsafe.
      */
    void disconnected();

    /*!
      This signal should be emitted each time a new message is available.

      \note
      __This signal might be unsafe__ (read this: \ref safe-signal)!\n\n

      Unless you know what subclass of AbstractMessageSocket is being used
      __AND__ this subclass explicitly documents the opposite (signal is safe),
      you should assume that this signal is unsafe.
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
    bool connected_;
};

} // namespace Tufao

#endif // TUFAO_ABSTRACTMESSAGEDISPATCHER_H
