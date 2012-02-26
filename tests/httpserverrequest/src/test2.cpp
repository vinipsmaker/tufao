#include "test2.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

#ifndef TIMEOUT_CLIENT_TIME
# error TIMEOUT_CLIENT_TIME not defined
#endif

/*
  Reference message:

  GET /a/b/c/x/y/z HTTP/0.9\r\n
  Host: localhost:8080\r\n
  UserDefined: test2\r\n
  \r\n
  */

Test2::Test2(QObject *parent) :
    AbstractTest(parent)
{
}

void Test2::operator ()()
{
    qDebug("Running test2");
    QList<QByteArray> data;
    data << "GE" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "0" << "." << "9" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "User" << "Defined: " << "test" << "2" << "\r" << "\n" << "\r"
         << "\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer.start(TIMEOUT_CLIENT_TIME);
}

void Test2::testRequest(Tufao::HttpServerRequest *,
                        Tufao::HttpServerResponse *)
{
    qFatal("Connection not closed on unsupported HTTP version");
}

void Test2::onDisconnected()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    socket->deleteLater();
    socket = NULL;
    qDebug("test2 OK");
    emit ready();
}

void Test2::onTimer()
{
    qFatal("Connection not closed on unsupported HTTP version");
}
