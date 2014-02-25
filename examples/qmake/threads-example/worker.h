#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QMutex>

#include "tcpserver.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker();

    void setFactory(TcpServer::Factory factory);
    void addConnection(qintptr socketDescriptor);

signals:
    void newConnection(qintptr socketDescriptor);
    void initReady();

private slots:
    void init();
    void onNewConnection(qintptr socketDescriptor);
    void onRequestReady();
    void onUpgrade();

private:
    TcpServer::Factory factory;
    QMutex factoryMutex;
    TcpServer::Handler handler;
};

#endif // WORKER_H
