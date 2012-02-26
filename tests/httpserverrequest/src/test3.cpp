#include "test3.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

#ifndef TIMEOUT_CLIENT_TIME
# error TIMEOUT_CLIENT_TIME not defined
#endif

/*
  Reference message:

  GET /a/b/c/x/y/z HTTP/1.2\r\n
  Host: localhost:8080\r\n
  UserDefined: test3\r\n
  \r\n
  */

Test3::Test3(QObject *parent) :
    AbstractTest(parent)
{
}

void Test3::operator ()()
{
    qDebug("Running test3");
    QList<QByteArray> data;
    data << "GE" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "2" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "User" << "Defined: " << "test" << "3" << "\r" << "\n" << "\r"
         << "\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer.start(TIMEOUT_CLIENT_TIME);
}

void Test3::testRequest(Tufao::HttpServerRequest *,
                        Tufao::HttpServerResponse *)
{
    qFatal("Connection not closed on unsupported HTTP version");
}

void Test3::onDisconnected()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    socket->deleteLater();
    socket = NULL;
    qDebug("test3 OK");
    emit ready();
}

void Test3::onTimer()
{
    qFatal("Connection not closed on unsupported HTTP version");
}
