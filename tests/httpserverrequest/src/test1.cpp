#include "test1.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

/*
  Reference message:

  GET /a/b/c/x/y/z HTTP/1.1\r\n
  Host: localhost:8080\r\n
  UserDefined: test1\r\n
  \r\n
  */

Test1::Test1(QObject *parent) :
    AbstractTest(parent)
{
}

void Test1::operator ()()
{
    qDebug("Running test1");
    QList<QByteArray> data;
    data << "GE" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "1" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "User" << "Defined: " << "test" << "1" << "\r" << "\n" << "\r"
         << "\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(testReady()), this, SLOT(onReady()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Test1::testRequest(Tufao::HttpServerRequest *request,
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
            Q_ASSERT(i.value() == "test1");
        } else {
            qFatal("Unexpected header");
        }
    }

    Tufao::HttpServerResponse::Options options = response->options();
    Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::KEEP_ALIVE));
    Q_ASSERT(!options.testFlag(Tufao::HttpServerResponse::HTTP_1_0));
    Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::HTTP_1_1));
    qDebug("test1 OK");
    emit ready();
}

void Test1::onReady()
{
    socket->close();
    socket = NULL;
}
