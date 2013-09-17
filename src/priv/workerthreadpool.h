#ifndef TUFAO_WORKERTHREADPOOL_H
#define TUFAO_WORKERTHREADPOOL_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QMap>
#include <QtCore/QQueue>

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
        WorkerThreadPool(QObject* parent);

        bool handleConnection    (quintptr socketDesc);
        void shutdown            ();
        void stopAllThreads      ();
        void pauseDispatch       (const bool pause = true);

        void setConnectionHandlerFactory (WorkerThreadData::ConnHandlerFactory factory);
        void setRequestHandlerFactory    (WorkerThreadData::RequestHandlerFactory factory);
        void setCleanupHandlerFactory    (WorkerThreadData::CleanupHandlerFactory factory);

        const WorkerThreadData::ConnHandlerFactory &connectionHandlerFactory() const;

        int getPoolSize() const;
        void setPoolSize(int value);

        void          unregisterIdleThread (WorkerThread* thread);
        void          registerIdleThread   (WorkerThread* thread);

    public slots:
        void          startThreads     ();

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
