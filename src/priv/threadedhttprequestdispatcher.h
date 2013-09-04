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

#ifndef TUFAO_PRIV_THREADEDHTTPREQUESTDISTPATCHER_H
#define TUFAO_PRIV_THREADEDHTTPREQUESTDISTPATCHER_H

#include "../threadedhttprequestdispatcher.h"

#include <QtCore/QVector>
#include <QtCore/QThread>
#include <QtCore/QRunnable>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QAtomicInt>
#include <QtCore/QQueue>

#include "workerthread.h"

namespace Tufao {

struct ThreadedHttpRequestDispatcher::Priv
{
    Priv(ThreadedHttpRequestDispatcher* pub);
    virtual ~Priv() = default;

    void dispatchRequests ();
    void stopAllThreads   ();

    QList< WorkerThread* >    idleThreads;
    QMap<int,WorkerThread* >  workingThreads;
    Factory                   threadInitializer;
    CleanupFunc               threadCleaner;

    QQueue<WorkerThread::Request> pendingRequests;
    unsigned int                  numberOfThreads;
    bool                          deferredDispatch;
    bool                          rejectRequests;

    ThreadedHttpRequestDispatcher* pub;


};

} // namespace Tufao

#endif // TUFAO_PRIV_THREADEDHTTPREQUESTDISTPATCHER_H
