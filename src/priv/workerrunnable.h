/*  This file is part of the Tufão project
    Copyright (C) 2013 Benjamin Zeller

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
#ifndef TUFAO_WORKERRUNNABLE_H
#define TUFAO_WORKERRUNNABLE_H

#include <QObject>
#include <QMutex>
#include <QQueue>

class QTimer;

namespace Tufao {

class AbstractConnectionHandler;
class HttpServerRequest;
class HttpServerResponse;
class WorkerThread;

class WorkerRunnable : public QObject
{
        Q_OBJECT
    public:

        enum Load{
            Low,
            Medium,
            High
        };

        explicit WorkerRunnable(WorkerThread* thread,AbstractConnectionHandler* handler, QObject *parent = 0);

        Load workLoad () const;
        void handleConnection (qintptr socketDesc);

    public slots:
        void onShutdownRequested ();
        
    signals:
        
    private slots:
        void flushQueue ();
        void onResponsiveTimer ();
        void onNewRequest  (HttpServerRequest &request);
        void onRequestReady();
        void onRequestEnd();
        void onRequestDestroyed();

    private:
        WorkerThread*              thread;
        AbstractConnectionHandler* connHandler;

        mutable QMutex  lastResponseMutex;
        quint64 lastResponseTimeout;

        QMutex          queueMutex;
        QQueue<qintptr> pendingConnections;


        QTimer *responsiveTimer;
        int currentRequestCount; //@TODO check if using a QAtomicInt could work
};

} // namespace Tufao

#endif // TUFAO_WORKERRUNNABLE_H
