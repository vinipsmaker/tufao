#include "workerthreadcontrol.h"
#include "threadedhttprequestdispatcher.h"
#include <QThread>
#include <QDebug>

namespace Tufao {

WorkerThreadControl::WorkerThreadControl(QObject *parent) :
    QObject(parent)
{
}

void WorkerThreadControl::onResponseFinished()
{
    tDebug()<<"onResponseFinished";
    QThread::currentThread()->exit();
}

void WorkerThreadControl::onRequestDestroyed()
{
    tDebug()<<"onRequestDestroyed";
    QThread::currentThread()->exit();
}

void WorkerThreadControl::onRequestClosed()
{
    tDebug()<<"onRequestClosed";
    QThread::currentThread()->exit();
}

}
