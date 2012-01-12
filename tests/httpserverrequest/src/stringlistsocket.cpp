#include "stringlistsocket.h"
#include <QHostAddress>

#ifndef WAIT_TIME
# error WAIT_TIME not defined
#endif

StringListSocket::StringListSocket(const QList<QByteArray> &data, QObject *parent) :
    QTcpSocket(parent),
    data(data),
    index(0)
{
    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
    connectToHost(QHostAddress::LocalHost, 8080);

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

void StringListSocket::onConnected()
{
    timer.start(WAIT_TIME);
}

void StringListSocket::onTimer()
{
    write(data[index++]);
    if (index == data.size()) {
        disconnect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
        connect(&timer, SIGNAL(timeout()), this, SIGNAL(testReady()));
    }
    timer.start(WAIT_TIME);
}
