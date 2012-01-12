#include "testsguide.h"
#include "test1.h"
#include <httpserverrequest.h>
#include <headers.h>
#include <QCoreApplication>

TestsGuide::TestsGuide(QObject *parent) :
    QObject(parent),
    completed(0)
{
    tests << new Test1();

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
        qDebug("test1 OK");
    } else {
        QCoreApplication::instance()->exit(1);
    }
}

void TestsGuide::onTestReady()
{
    ++completed;
    if (tests.size() == completed)
        QCoreApplication::instance()->exit();
}
