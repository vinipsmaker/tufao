#include "test5.h"
#include <httpserverrequest.h>
#include <headers.h>

#ifndef TIMEOUT_CLIENT_TIME
# error TIMEOUT_CLIENT_TIME not defined
#endif

/*
  Reference message:

  GET /a/b/c/x/y/z HTTP/1.1\r\n
  Host: localhost:8080\r\n
  Connection: close\r\n
  UserDefined: test5\r\n
  \r\n
  */

Test5::Test5(QObject *parent) :
    AbstractTest(parent)
{
}

void Test5::operator ()()
{
    qDebug("Running test5");
    QList<QByteArray> data;
    data << "GE" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "1" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "Conn" << "ection" << ": cl" << "ose" << "\r\n"
         << "User" << "Defined: " << "test" << "5" << "\r" << "\n" << "\r"
         << "\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000
           + TIMEOUT_SERVER_TIME/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer.start(TIMEOUT_SERVER_TIME);
}

void Test5::testRequest(Tufao::HttpServerRequest *request,
                        Tufao::HttpServerResponse *response)
{
    Q_ASSERT(request->method() == "GET");
    Q_ASSERT(request->url() == "/a/b/c/x/y/z");
    Q_ASSERT(request->httpVersion() == Tufao::HttpServerRequest::HTTP_1_1);

    Tufao::Headers headers = request->headers();
    for (Tufao::Headers::iterator i = headers.begin();i != headers.end();++i) {
        if (i.key() == Tufao::IByteArray("Host")) {
            Q_ASSERT(i.value() == "localhost:8080");
        } else if (i.key() == Tufao::IByteArray("UserDefined")) {
            Q_ASSERT(i.value() == "test5");
        } else if (i.key() == Tufao::IByteArray("Connection")) {
            Q_ASSERT(i.value() == "close");
        } else {
            qFatal("Unexpected header");
        }
    }

    Tufao::HttpServerResponse::Options options = response->options();
    Q_ASSERT(!options.testFlag(Tufao::HttpServerResponse::KEEP_ALIVE));
    Q_ASSERT(!options.testFlag(Tufao::HttpServerResponse::HTTP_1_0));
    Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::HTTP_1_1));

    response->writeHead(Tufao::HttpServerResponse::OK);
    // So the connection should just TIMEOUT
    //response->end("42\n");
}

void Test5::onDisconnected()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    socket->deleteLater();
    socket = NULL;
    qDebug("test5 OK");
    emit ready();
}

void Test5::onTimer()
{
    qFatal("Connection not closed on timeout");
}
