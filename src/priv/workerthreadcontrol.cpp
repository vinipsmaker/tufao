#include "workerthreadcontrol.h"
#include <QThread>
#include <QDebug>

#define debug() qDebug()<<"["<<QThread::currentThreadId()<<"] "

WorkerThreadControl::WorkerThreadControl(QObject *parent) :
    QObject(parent)
{
}

void WorkerThreadControl::onResponseFinished()
{
    debug()<<"onResponseFinished";
    QThread::currentThread()->exit();
}

void WorkerThreadControl::onRequestDestroyed()
{
    debug()<<"onRequestDestroyed";
    QThread::currentThread()->exit();
}

void WorkerThreadControl::onRequestEnd()
{
    debug()<<"onRequestEnd";
    QThread::currentThread()->exit();
}

void WorkerThreadControl::onRequestClosed()
{
    debug()<<"onRequestClosed";
    QThread::currentThread()->exit();
}
