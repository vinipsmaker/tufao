#include "testsguide.h"
#include "test1.h"
#include "test2.h"
#include "test3.h"
#include "test4.h"
#include "test6.h"
#include "test9.h"
#include "test10.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>
#include <QCoreApplication>
#include <cstdlib>

TestsGuide::TestsGuide(QObject *parent) :
    HttpServer(parent),
    completed(0),
    upgradeTest(new Test8)
{
    Q_ASSERT(listen(QHostAddress::Any, 8080));

    connect(this, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
            this, SLOT(onRequestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

    tests << new Test1 << new Test2 << new Test3 << new Test4
          << new Test6 << upgradeTest << new Test9 << new Test10;

    for (int i = 0;i != tests.size();++i) {
        connect(tests[i], SIGNAL(ready()), this, SLOT(onTestReady()));
        (*tests[i])();
    }
}

void TestsGuide::onRequestReady(Tufao::HttpServerRequest *request,
                                Tufao::HttpServerResponse *response)
{
    if (request->headers().value("UserDefined") == "test1") {
        tests[0]->testRequest(request, response);
    } else if (request->headers().value("UserDefined") == "test2") {
        tests[1]->testRequest(request, response);
    } else if (request->headers().value("UserDefined") == "test3") {
        tests[2]->testRequest(request, response);
    } else if (request->headers().value("UserDefined") == "test4") {
        tests[3]->testRequest(request, response);
    } else if (request->headers().value("UserDefined") == "test6") {
        tests[4]->testRequest(request, response);
    } else if (request->headers().value("UserDefined") == "test9") {
        tests[6]->testRequest(request, response);
    } else if (request->headers().value("UserDefined") == "test10") {
        tests[7]->testRequest(request, response);
    } else if (request->headers().value("UserDefined") == "test10") {
        tests[7]->testRequest(request, response);
    } else {
        qFatal("Unexpected header");
    }
}

void TestsGuide::onTestReady()
{
    if (tests.size() == ++completed)
        exit(0);
}

void TestsGuide::upgrade(Tufao::HttpServerRequest *request, const QByteArray &head)
{
    Q_ASSERT(request->method() == "GET");
    Q_ASSERT(request->url() == "/a/b/c/x/y/z");
    Q_ASSERT(request->httpVersion() == Tufao::HttpServerRequest::HTTP_1_1);

    Tufao::Headers headers = request->headers();
    for (Tufao::Headers::iterator i = headers.begin();i != headers.end();++i) {
        if (i.key() == Tufao::IByteArray("Host")) {
            Q_ASSERT(i.value() == "localhost:8080");
        } else if (i.key() == Tufao::IByteArray("Connection")) {
            Q_ASSERT(i.value() == "Upgrade");
        } else if (i.key() == Tufao::IByteArray("Upgrade")) {
            Q_ASSERT(i.value() == "Dummy");
        } else if (i.key() == Tufao::IByteArray("UserDefined")) {
            Q_ASSERT(i.value() == "test8");
        } else {
            qFatal("Unexpected header");
        }
    }
    Q_ASSERT(head == "42");
    qDebug("test8 OK");
    onTestReady();
}
