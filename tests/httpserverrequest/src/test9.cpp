#include "test9.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

/*
  Reference message:

  POST /a/b/c/x/y/z HTTP/1.1\r\n
  Host: localhost:8080\r\n
  UserDefined: test9\r\n
  Transfer-Encoding: chunked\r\n
  \r\n
  2\r\n
  42\r\n
  0\r\n
  Content-MD5: 50a2fabfdd276f573ff97ace8b11c5f4\r\n
  \r\n
  */

Test9::Test9(QObject *parent) :
    AbstractTest(parent),
    ok(false)
{
}

void Test9::operator ()()
{
    qDebug("Running test9");
    QList<QByteArray> data;
    data << "POS" << "T" << " " << "/a/b/c" << "/x/y/z" << " " << "HT" << "T"
         << "P/" << "1" << "." << "1" << "\r" << "\n"
         << "Ho" << "st" << ":" << " " << "localhos" << "t:8080" << "\r" << "\n"
         << "User" << "Defined: " << "test" << "9" << "\r" << "\n"
         << "Transfer-Encoding: chunked\r\n" << "\r\n"
         << "2\r\n42\r\n0\r\n"
         << "Content" << "-MD5: 50a2fabfdd276" << "f573ff97ace8b11c5f4\r\n\r\n";
    qDebug("Estimated time: %i seconds", (data.size() * WAIT_TIME)/1000);
    socket = new StringListSocket(data, this);
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Test9::testRequest(Tufao::HttpServerRequest *request,
                        Tufao::HttpServerResponse *response)
{
    Q_ASSERT(request->method() == "POST");
    Q_ASSERT(request->url() == "/a/b/c/x/y/z");
    Q_ASSERT(request->httpVersion() == Tufao::HttpServerRequest::HTTP_1_1);

    Tufao::Headers headers = request->headers();
    for (Tufao::Headers::iterator i = headers.begin();i != headers.end();++i) {
        if (i.key() == Tufao::IByteArray("Host")) {
            Q_ASSERT(i.value() == "localhost:8080");
        } else if (i.key() == Tufao::IByteArray("UserDefined")) {
            Q_ASSERT(i.value() == "test9");
        } else if (i.key() == Tufao::IByteArray("Transfer-Encoding")) {
            Q_ASSERT(i.value() == "chunked");
        } else {
            qFatal("Unexpected header");
        }
    }

    Tufao::HttpServerResponse::Options options = response->options();
    Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::KEEP_ALIVE));
    Q_ASSERT(!options.testFlag(Tufao::HttpServerResponse::HTTP_1_0));
    Q_ASSERT(options.testFlag(Tufao::HttpServerResponse::HTTP_1_1));

    connect(request, SIGNAL(data(QByteArray)), this, SLOT(onData(QByteArray)));
    connect(request, SIGNAL(end()), this, SLOT(onEnd()));
    connect(request->socket(), SIGNAL(destroyed()), this, SLOT(onFail()));
    connect(request->socket(), SIGNAL(disconnected()), this, SLOT(onFail()));
    connect(request, SIGNAL(destroyed()), this, SLOT(onFail()));

    this->request = request;
    this->response = response;
}

void Test9::onData(const QByteArray &chunk)
{
    buffer += chunk;
}

void Test9::onEnd()
{
    Q_ASSERT(buffer == "42");
    Tufao::Headers trailers = request->trailers();
    for (Tufao::Headers::iterator i = trailers.begin()
         ;i != trailers.end();++i) {
        if (i.key() == Tufao::IByteArray("Content-MD5")) {
            Q_ASSERT(i.value() == "50a2fabfdd276f573ff97ace8b11c5f4");
        } else {
            qFatal("Unexpected trailer");
        }
    }
    ok = true;
    qDebug("test9 OK");
    emit ready();
}

void Test9::onFail()
{
    Q_ASSERT(ok);
}
