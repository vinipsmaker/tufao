#include "test10.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

/*
  Reference message:

  HEAD /a/b/c/x/y/z HTTP/1.1\r\n
  Host: localhost:8080\r\n
  UserDefined: test10\r\n
  \r\n
  GET /2 HTTP/1.0\r\n
  UserDefined: test10\r\n
  \r\n
  */

Test10::Test10(QObject *parent) :
    AbstractTest(parent),
    firstRequest(true)
{
}

void Test10::operator ()()
{
    qDebug("Running test10");
    QList<QByteArray> data;
    data << "HE" << "AD" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "1" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "User" << "Defined: " << "test" << "10" << "\r" << "\n" << "\r"
         << "\n"
         << "GET /2 HTTP/1.0\r\n" << "UserDefined: test10\r\n\r\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Test10::testRequest(Tufao::HttpServerRequest *request,
                        Tufao::HttpServerResponse *response)
{
    if (firstRequest) {
        Q_ASSERT(request->method() == "HEAD");
        Q_ASSERT(request->url() == "/a/b/c/x/y/z");
        Q_ASSERT(request->httpVersion() == Tufao::HttpServerRequest::HTTP_1_1);

        Tufao::Headers headers = request->headers();
        for (Tufao::Headers::iterator i = headers.begin();i != headers.end();++i) {
            if (i.key() == Tufao::IByteArray("Host")) {
                Q_ASSERT(i.value() == "localhost:8080");
            } else if (i.key() == Tufao::IByteArray("UserDefined")) {
                Q_ASSERT(i.value() == "test10");
            } else {
                qFatal("Unexpected header");
            }
        }

        Tufao::HttpServerResponse::Options options = response->options();
        Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::KEEP_ALIVE));
        Q_ASSERT(!options.testFlag(Tufao::HttpServerResponse::HTTP_1_0));
        Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::HTTP_1_1));
        firstRequest = false;
    } else {
        Q_ASSERT(request->method() == "GET");
        Q_ASSERT(request->url() == "/2");
        Q_ASSERT(request->httpVersion() == Tufao::HttpServerRequest::HTTP_1_0);

        Tufao::Headers headers = request->headers();
        for (Tufao::Headers::iterator i = headers.begin();i != headers.end();++i) {
            if (i.key() == Tufao::IByteArray("UserDefined")) {
                Q_ASSERT(i.value() == "test10");
            } else {
                qFatal("Unexpected header");
            }
        }

        qDebug("test10 OK");
        emit ready();
    }
}
