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

#include "workerthreadpool.h"
#include "workerthread.h"
#include "tcpserverwrapper.h"

namespace Tufao{

struct ThreadedHttpServer::Priv{
        Priv(ThreadedHttpServer* pub);
        virtual ~Priv() = default;

        TcpServerWrapper    tcpServer;
        WorkerThreadPool    *threadPool;
        bool                rejectRequests;
        ThreadedHttpServer  *pub;

        static ThreadedHttpServer::ConnectionHandlerFactory defConnHdl;
};

}

#endif // TUFAO_PRIV_THREADEDHTTPSERVER_P_H
