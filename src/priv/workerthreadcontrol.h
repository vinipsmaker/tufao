#ifndef THREADLOGGER_H
#define THREADLOGGER_H

#include <QObject>

class WorkerThreadControl : public QObject
{
        Q_OBJECT
    public:
        explicit WorkerThreadControl(QObject *parent = 0);
        
    signals:
        
    public slots:
        void onResponseFinished();
        void onRequestDestroyed();
        void onRequestEnd ();
        void onRequestClosed();
        
};

#endif // THREADLOGGER_H
