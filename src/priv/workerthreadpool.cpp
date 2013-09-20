#include "workerthreadpool.h"
#include "workerthread.h"
#include "../abstractconnectionhandler.h"

#include <functional>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QScopedPointer>

namespace Tufao {

WorkerThreadPool::WorkerThreadPool(QObject *parent)
    : QObject(parent)
    , poolSize(10)
    , deferredDispatch(false)
{
}

void WorkerThreadPool::dispatchRequests()
{
    if(deferredDispatch)
        return;

    qDebug()<<"Starting request dispatch";
    while(pendingConnections.size()){

        WorkerThread* w = findResponsiveThread();
        if(!w){
            qDebug()<<"!!!!!!!!!!! We ran out of threads !!!!!!!!!!!!!!!!!!!!!!!!!!!";
            staticMetaObject.invokeMethod(this,"dispatchRequests",Qt::QueuedConnection);
            break;
        }

        quintptr socketDesc = pendingConnections.dequeue();
        w->handleRequest(socketDesc);
    }
    qDebug()<<"Finished request dispatch";
}

void WorkerThreadPool::stopAllThreads()
{
    QTime timer;
    long timeout = 60 * 1000;

    timer.start();
#if 0
    while(true){

        int size = 0;
        threadListMutex.lock();
        size = workingThreads.size();
        threadListMutex.unlock();

        if(size <= 0)
            break;

        //deliver thread finished events to us
        //QCoreApplication::sendPostedEvents(pub,WorkerThreadEvent::ThreadIdle);
        QThread::msleep(10);

        /* If we hit this, we waited long enough, maybe the threads
         * are somehow blocked. We need to force quit them
         */
        if(timer.elapsed() > timeout){
            threadListMutex.lock();
            for(auto i = workingThreads.begin();
                i != workingThreads.end(); i++){

                WorkerThread* th = i.value();
                th->terminate();
                th->wait(1000);
                delete th;

            }
            workingThreads.clear();
            threadListMutex.unlock();
            break;
        }

    }
#endif
    timeout = 10 * 1000;

    //quit all threads
    QMutexLocker l(&threadListMutex);
    for(int i= 0; i < threads.size(); i++){
        WorkerThread* th = threads.takeAt(i);
        th->shutdown();
        bool finished = th->wait(timeout);
        if(!finished){
            th->terminate();
            finished = th->wait(timeout);

            //@BUG what to do if the thread can not be terminated
            if(!finished)
                qDebug()<<"Could not stop thread correctly";
        }
        delete th;
    }
}

void WorkerThreadPool::pauseDispatch(const bool pause)
{
    deferredDispatch = pause;

    if(!deferredDispatch)
        staticMetaObject.invokeMethod(this,"dispatchRequests",Qt::QueuedConnection);
}

bool WorkerThreadPool::isInitialized() const
{
    return (threads.size() > 0);
}

void WorkerThreadPool::setConnectionHandlerFactory(const WorkerThreadData::ConnHandlerFactory &factory)
{
    threadInit.connHandlerFactory = factory;
}

void WorkerThreadPool::setRequestHandlerFactory(const WorkerThreadData::RequestHandlerFactory &factory)
{
    threadInit.reqHandlerfactory = factory;
}

void WorkerThreadPool::setCleanupHandlerFactory(const WorkerThreadData::CleanupHandlerFactory &factory)
{
    threadInit.cleanup = factory;
}

const WorkerThreadData::ConnHandlerFactory &WorkerThreadPool::connectionHandlerFactory() const
{
    return threadInit.connHandlerFactory;
}

const WorkerThreadData::RequestHandlerFactory &WorkerThreadPool::requestHandlerFactory() const
{
    return threadInit.reqHandlerfactory;
}

const WorkerThreadData::CleanupHandlerFactory &WorkerThreadPool::cleanupHandlerFactory() const
{
    return threadInit.cleanup;
}

void WorkerThreadPool::start()
{
    extStart(threadInit.connHandlerFactory,threadInit.reqHandlerfactory,threadInit.cleanup);
}

/*!
 * \brief WorkerThreadPool::findResponsiveThread
 * \return
 */
WorkerThread *WorkerThreadPool::findResponsiveThread()
{
    QMutexLocker l(&threadListMutex);

    if(idleThreads.size()){
        WorkerThread* wt = idleThreads.takeFirst();
        qDebug()<<"[Main] Using Thread "<<wt->threadId()<<" from idle List";
        return wt;
    }else{
        qDebug()<<"[Main] No Idle Threads trying to find a responsive one";

        int myThread = -1;
        for(int i = 0; i < threads.size(); i++){
            WorkerRunnable::Load load = threads.at(i)->workLoad();
            if(load == WorkerRunnable::Low){
                myThread = i;
                break;
            }else if(load == WorkerRunnable::Medium && myThread == -1){
                myThread = i;
                continue;
            }
        }

        if(myThread >= 0){
            //move the thread to the back, so it will be picked later the next time
            WorkerThread* t = threads.takeAt(myThread);
            threads.append(t);

            return t;
        }

        //we did not find a thread , enqueue it
    }
    return 0;
}

void WorkerThreadPool::unregisterIdleThread(WorkerThread *thread)
{
    QMutexLocker l(&threadListMutex);
    idleThreads.removeAll(thread);
}

void WorkerThreadPool::registerIdleThread(WorkerThread *thread)
{
    QMutexLocker l(&threadListMutex);

    if(!idleThreads.contains(thread))
        idleThreads.append(thread);

    qDebug()<<"[Main] Threads in Idle Mode: "<<idleThreads.size();

    l.unlock();

    if(pendingConnections.size())
        staticMetaObject.invokeMethod(this,"dispatchRequests",Qt::QueuedConnection);
}

/*!
 * \internal
 * \brief WorkerThreadPool::extStart
 *  Start the threadpool with special factories
 * \param cf
 * \param rF
 * \param clF
 */
void WorkerThreadPool::extStart(const WorkerThreadData::ConnHandlerFactory &cF
                                ,const  WorkerThreadData::RequestHandlerFactory &rF
                                ,const  WorkerThreadData::CleanupHandlerFactory &clF)
{
    if(threads.size())
        return;

    threadListMutex.lock();

    Q_ASSERT_X(rF,Q_FUNC_INFO,"You have to set a request handler factory");

    for(int i = 0; i < poolSize; i++){
        WorkerThread *w = new WorkerThread(i,
                 cF,
                 rF,
                 clF,
                 this);
        w->start();
        threads.append(w);
    }
    threadListMutex.unlock();
}

int WorkerThreadPool::getPoolSize() const
{
    return poolSize;
}

void WorkerThreadPool::setPoolSize(int value)
{
    poolSize = value;
}


bool WorkerThreadPool::handleConnection(quintptr socketDesc)
{
    if(!threads.size()){
        QScopedPointer<AbstractConnectionHandler> handler(threadInit.connHandlerFactory());
        handler->closePendingConnection(pendingConnections.takeFirst());
        return false;
    }

    pendingConnections.append(socketDesc);
    staticMetaObject.invokeMethod(this,"dispatchRequests",Qt::QueuedConnection);
    return true;
}

void WorkerThreadPool::shutdown()
{
    QScopedPointer<AbstractConnectionHandler> handler(threadInit.connHandlerFactory());

    while(pendingConnections.size()){
        handler->closePendingConnection(pendingConnections.takeFirst());
    }

    stopAllThreads();
}

} // namespace Tufao
