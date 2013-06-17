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
        << QDateTime(QDate(1994, 11, 6), QTime(8, 49, 37), Qt::UTC)
        << QByteArray("Sun, 06 Nov 1994 08:49:37 GMT");
    QTest::newRow("Another data")
        << QDateTime(QDate(1903, 12, 1), QTime(), Qt::UTC)
        << QByteArray("Tue, 01 Dec 1903 00:00:00 GMT");
    QTest::newRow("Yet another data")
        << QDateTime(QDate(2003, 12, 1), QTime(), Qt::UTC)
        << QByteArray("Mon, 01 Dec 2003 00:00:00 GMT");
}

void Rfc1123Test::dateTimeConversion()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QByteArray, rfc1123);

    const int locales_count = 3;
    QLocale locales[locales_count] = {
        QLocale(QLocale::C),
        QLocale(QLocale::Portuguese),
        QLocale(QLocale::Dutch)
    };

    for (int i = 0;i != locales_count;++i) {
        QLocale::setDefault(locales[i]);

        Rfc1123 object(rfc1123);
        QVERIFY(static_cast<bool>(object));
        QCOMPARE(object(), datetime);
    }
}

QTEST_APPLESS_MAIN(Rfc1123Test)
