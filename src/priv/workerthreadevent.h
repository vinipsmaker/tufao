#ifndef TUFAO_PRIV_WORKERTHREADEVENT_H
#define TUFAO_PRIV_WORKERTHREADEVENT_H

#include <QEvent>

namespace Tufao{

class WorkerThread;

class WorkerThreadEvent : public QEvent
{
    public:
        enum Type{
            ThreadStarted  = QEvent::User + 1000,
            ThreadRunning  = ThreadStarted+1,
            ThreadIdle     = ThreadRunning+1
        };

        WorkerThreadEvent(Type t);

        WorkerThread* thread;
};

} //namespace Tufao

#endif // TUFAO_PRIV_WORKERTHREADEVENT_H
