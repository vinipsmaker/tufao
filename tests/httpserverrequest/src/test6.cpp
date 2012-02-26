#include "test6.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

#ifndef TIMEOUT_CLIENT_TIME
# error TIMEOUT_CLIENT_TIME not defined
#endif

/*
  Reference message:

  GET /a/b/c/x/y/z HTTP/1.0\r\n
  Host: localhost:8080\r\n
  UserDefined: test6\r\n
  \r\n
  */

Test6::Test6(QObject *parent) :
    AbstractTest(parent)
{
}

void Test6::operator ()()
{
    qDebug("Running test6");
    QList<QByteArray> data;
    data << "GE" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "0" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "User" << "Defined: " << "test" << "6" << "\r" << "\n" << "\r"
         << "\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer.start(TIMEOUT_CLIENT_TIME);
}

void Test6::testRequest(Tufao::HttpServerRequest *request,
                        Tufao::HttpServerResponse *response)
{
    Q_ASSERT(request->method() == "GET");
    Q_ASSERT(request->url() == "/a/b/c/x/y/z");
    Q_ASSERT(request->httpVersion() == Tufao::HttpServerRequest::HTTP_1_0);

    Tufao::Headers headers = request->headers();
    for (Tufao::Headers::iterator i = headers.begin();i != headers.end();++i) {
        if (i.key() == Tufao::IByteArray("Host")) {
            Q_ASSERT(i.value() == "localhost:8080");
        } else if (i.key() == Tufao::IByteArray("UserDefined")) {
            Q_ASSERT(i.value() == "test6");
        } else if (i.key() == Tufao::IByteArray("Connection")) {
            Q_ASSERT(i.value() == "close");
        } else {
            qFatal("Unexpected header");
        }
    }

    Tufao::HttpServerResponse::Options options = response->options();
    Q_ASSERT(!options.testFlag(Tufao::HttpServerResponse::KEEP_ALIVE));
    Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::HTTP_1_0));
    Q_ASSERT(!options.testFlag(Tufao::HttpServerResponse::HTTP_1_1));

    response->writeHead(Tufao::HttpServerResponse::OK);
    response->end("42\n");
}

void Test6::onDisconnected()
{
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    socket->deleteLater();
    socket = NULL;
    qDebug("test6 OK");
    emit ready();
}

void Test6::onTimer()
{
    qFatal("Connection not closed on HTTP/1.0");
}
