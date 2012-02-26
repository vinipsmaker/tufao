#include "test4.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

#ifndef TIMEOUT_CLIENT_TIME
# error TIMEOUT_CLIENT_TIME not defined
#endif

/*
  Reference message:

  GET /a/b/c/x/y/z HTTP/1.1\r\n
  Host: localhost:8080\r\n
  Connection: close\r\n
  UserDefined: test4\r\n
  \r\n
  */

Test4::Test4(QObject *parent) :
    AbstractTest(parent)
{
}

void Test4::operator ()()
{
    qDebug("Running test4");
    QList<QByteArray> data;
    data << "GE" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "1" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "Conn" << "ection" << ": cl" << "ose" << "\r\n"
         << "User" << "Defined: " << "test" << "4" << "\r" << "\n" << "\r"
         << "\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer.start(TIMEOUT_CLIENT_TIME);
}

void Test4::testRequest(Tufao::HttpServerRequest *request,
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
            Q_ASSERT(i.value() == "test4");
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
    response->end("42\n");
}

void Test4::onDisconnected()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    socket->deleteLater();
    socket = NULL;
    qDebug("test4 OK");
    emit ready();
}

void Test4::onTimer()
{
    qFatal("Connection not closed on Connection: close");
}
