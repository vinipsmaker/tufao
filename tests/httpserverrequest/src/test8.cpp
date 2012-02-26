#include "test8.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

/*
  Reference message:

  GET /a/b/c/x/y/z HTTP/1.1\r\n
  Host: localhost:8080\r\n
  Connection: Upgrade\r\n
  Upgrade: Dummy\r\n
  UserDefined: test8\r\n
  \r\n
  42
  */

Test8::Test8(QObject *parent) :
    AbstractTest(parent)
{
}

void Test8::operator ()()
{
    qDebug("Running test8");
    QList<QByteArray> data;
    data << "GE" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "1" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "Conn" << "ection: " << "Up" << "grade" << "\r\n"
         << "Upgrade: Dummy\r\n"
         << "User" << "Defined: " << "test" << "8" << "\r" << "\n" << "\r"
         << "\n42";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(testReady()), this, SLOT(onReady()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Test8::testRequest(Tufao::HttpServerRequest *,
                        Tufao::HttpServerResponse *)
{
    qFatal("Upgrade request not recognized");
}

void Test8::onReady()
{
    socket->close();
    socket = NULL;
}
