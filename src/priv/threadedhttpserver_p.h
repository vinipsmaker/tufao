#ifndef TUFAO_PRIV_THREADEDHTTPSERVER_P_H
#define TUFAO_PRIV_THREADEDHTTPSERVER_P_H

#include "../threadedhttpserver.h"
#include <QtCore/QVector>
#include <QtCore/QThread>
#include <QtCore/QRunnable>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QAtomicInt>
#include <QtCore/QQueue>

#include "workerthread.h"

namespace Tufao{

struct ThreadedHttpServer::Priv{
        Priv(ThreadedHttpServer* pub);
        virtual ~Priv() = default;

        void dispatchRequests ();
        void stopAllThreads   ();

        WorkerThread* takeIdleThread ();
        void          takeWorkingThread (WorkerThread* thread);

        QMutex                    threadListMutex;
        QList< WorkerThread* >    idleThreads;
        QMap<int,WorkerThread* >  workingThreads;


        Factory                   threadInitializer;
        CleanupFunc               threadCleaner;

        QQueue<qintptr>               pendingConnections;
        bool                          deferredDispatch;
        bool                          rejectRequests;

        ThreadedHttpServer* pub;

        int                       maxParallelThreads;
};

}

#endif // TUFAO_PRIV_THREADEDHTTPSERVER_P_H
