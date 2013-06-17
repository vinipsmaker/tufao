#include "asctime.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include "../priv/asctime.h"

using namespace Tufao;

void AsctimeTest::dateTimeConversion_data()
{
    QTest::addColumn<QDateTime>("datetime");
    QTest::addColumn<QByteArray>("asctime"); // ANSI C's asctime() format

    QTest::newRow("RFC 2616's 3.3.1")
        << QDateTime(QDate(1994, 11, 6), QTime(8, 49, 37), Qt::UTC)
        << QByteArray("Sun Nov  6 08:49:37 1994");
    QTest::newRow("Another data")
        << QDateTime(QDate(1903, 12, 1), QTime(), Qt::UTC)
        << QByteArray("Tue Dec  1 00:00:00 1903");
}

void AsctimeTest::dateTimeConversion()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QByteArray, asctime);

    const int locales_count = 3;
    QLocale locales[locales_count] = {
        QLocale(QLocale::C),
        QLocale(QLocale::Portuguese),
        QLocale(QLocale::Dutch)
    };

    for (int i = 0;i != locales_count;++i) {
        QLocale::setDefault(locales[i]);

        Asctime object(asctime);
        QVERIFY(static_cast<bool>(object));
        QCOMPARE(object(), datetime);
    }
}

QTEST_APPLESS_MAIN(AsctimeTest)
