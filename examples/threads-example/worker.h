#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include <Tufao/AbstractHttpServerRequestHandlerFactory>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(Tufao::AbstractHttpServerRequestHandlerFactory *factory);

    void addConnection(int socketDescriptor);

signals:
    void newConnection(int socketDescriptor);

private slots:
    void onNewConnection(int socketDescriptor);
    void onRequestReady();
    void onUpgrade(const QByteArray &);

private:
    Tufao::AbstractHttpServerRequestHandler *handler;
};

#endif // WORKER_H
