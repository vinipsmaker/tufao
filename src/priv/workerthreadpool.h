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
#ifndef TUFAO_WORKERTHREADPOOL_H
#define TUFAO_WORKERTHREADPOOL_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QMap>
#include <QtCore/QQueue>

#include <functional>

namespace Tufao {

class WorkerThread;
class AbstractConnectionHandler;
class AbstractHttpServerRequestHandler;

struct WorkerThreadData
{
    typedef std::function<AbstractConnectionHandler* ()> ConnHandlerFactory;
    typedef std::function<void (AbstractConnectionHandler*, void **)> RequestHandlerFactory;
    typedef std::function<void (void **customData)> CleanupHandlerFactory;

    ConnHandlerFactory  connHandlerFactory;
    RequestHandlerFactory reqHandlerfactory;
    CleanupHandlerFactory cleanup;
};

class WorkerThreadPool : public QObject
{
    Q_OBJECT

    public:
        WorkerThreadPool(QObject* parent = 0);

        bool handleConnection    (quintptr socketDesc);
        void shutdown            ();
        void stopAllThreads      ();
        void pauseDispatch       (const bool pause = true);
        bool isInitialized       () const;

        void setConnectionHandlerFactory (const WorkerThreadData::ConnHandlerFactory &factory);
        void setRequestHandlerFactory    (const WorkerThreadData::RequestHandlerFactory &factory);
        void setCleanupHandlerFactory    (const WorkerThreadData::CleanupHandlerFactory &factory);

        const WorkerThreadData::ConnHandlerFactory &connectionHandlerFactory() const;
        const WorkerThreadData::RequestHandlerFactory &requestHandlerFactory() const;
        const WorkerThreadData::CleanupHandlerFactory &cleanupHandlerFactory() const;

        int getPoolSize() const;
        void setPoolSize(int value);

        void          unregisterIdleThread (WorkerThread* thread);
        void          registerIdleThread   (WorkerThread* thread);

        void          extStart (const WorkerThreadData::ConnHandlerFactory &cF, const WorkerThreadData::RequestHandlerFactory &rF, const WorkerThreadData::CleanupHandlerFactory &clF );

    public slots:
        void          start ();

    protected slots:
        void          dispatchRequests ();

    protected:
        WorkerThread* findResponsiveThread ();

    private:
        int                       poolSize;
        int                       m_muhal;
        bool                      deferredDispatch;
        QMutex                    threadListMutex;
        QList< WorkerThread* >    idleThreads;
        QList< WorkerThread* >    threads;
        QQueue<qintptr>           pendingConnections;
        WorkerThreadData          threadInit;

    friend class WorkerThread;
};

} // namespace Tufao

#endif // TUFAO_WORKERTHREADPOOL_H
