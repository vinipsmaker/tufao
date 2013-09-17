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
