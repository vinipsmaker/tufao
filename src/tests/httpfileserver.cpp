#include "httpfileserver.h"
#include <QtTest/QTest>
#include "../httpfileserver.h"

using namespace Tufao;

void HttpFileServerTest::properties_data()
{
    QTest::addColumn<QString>("dir");

    QTest::newRow("bergamota") << "bergamota";
    QTest::newRow("furrovine") << "furrovine";
}

void HttpFileServerTest::properties()
{
    QFETCH(QString, dir);

    HttpFileServer fileserver(dir);

    QCOMPARE(fileserver.dir(), dir);

    fileserver.setDir("");

    QCOMPARE(fileserver.dir(), QString(""));
}

QTEST_APPLESS_MAIN(HttpFileServerTest)
