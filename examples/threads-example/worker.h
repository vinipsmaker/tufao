#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include "abstracthttpserverrequesthandlerfactory.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(AbstractHttpServerRequestHandlerFactory *factory);

    void addConnection(int socketDescriptor);

signals:
    void newConnection(int socketDescriptor);

private slots:
    void onNewConnection(int socketDescriptor);
    void onRequestReady();
    void onUpgrade();

private:
    AbstractHttpServerRequestHandlerFactory::Handler handler;
};

#endif // WORKER_H
