#include "rfc1123.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include "../priv/rfc1123.h"

using namespace Tufao;

void Rfc1123Test::dateTimeConversion_data()
{
    QTest::addColumn<QDateTime>("datetime");
    QTest::addColumn<QByteArray>("rfc1123");

    QTest::newRow("RFC 2616's 3.3.1")
        << QDateTime{QDate{1994, 11, 6}, QTime{8, 49, 37}, Qt::UTC}
        << QByteArray{"Sun, 06 Nov 1994 08:49:37 GMT"};
    QTest::newRow("Another data")
        << QDateTime{QDate{1903, 12, 1}, QTime{}, Qt::UTC}
        << QByteArray{"Tue, 01 Dec 1903 00:00:00 GMT"};
    QTest::newRow("Yet another data")
        << QDateTime{QDate{2003, 12, 1}, QTime{}, Qt::UTC}
        << QByteArray{"Mon, 01 Dec 2003 00:00:00 GMT"};
}

void Rfc1123Test::dateTimeConversion()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QByteArray, rfc1123);

    QLocale locales[]{
        {QLocale::C},
        {QLocale::Portuguese},
        {QLocale::Dutch}
    };

    for (auto locale: locales) {
        QLocale::setDefault(locale);

        QVERIFY(static_cast<bool>(Rfc1123{rfc1123}));
        QCOMPARE(Rfc1123{rfc1123}(), datetime);
    }
}

QTEST_APPLESS_MAIN(Rfc1123Test)
