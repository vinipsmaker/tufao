#include "rfc1036.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include "../priv/rfc1036.h"

using namespace Tufao;

void Rfc1036Test::dateTimeConversion_data()
{
    QTest::addColumn<QDateTime>("datetime");
    QTest::addColumn<QByteArray>("rfc1036");

    QTest::newRow("RFC 2616's 3.3.1")
        << QDateTime(QDate(1994, 11, 6), QTime(8, 49, 37), Qt::UTC)
        << QByteArray("Sunday, 06-Nov-94 08:49:37 GMT");
    QTest::newRow("Another data")
        << QDateTime(QDate(1903, 12, 1), QTime(), Qt::UTC)
        << QByteArray("Tuesday, 01-Dec-03 00:00:00 GMT");
}

void Rfc1036Test::dateTimeConversion()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QByteArray, rfc1036);

    const int locales_count = 3;
    QLocale locales[locales_count] = {
        QLocale(QLocale::C),
        QLocale(QLocale::Portuguese),
        QLocale(QLocale::Dutch)
    };

    for (int i = 0;i != locales_count;++i) {
        QLocale::setDefault(locales[i]);

        Rfc1036 object(rfc1036);
        QVERIFY(static_cast<bool>(object));
        QCOMPARE(object(), datetime);
    }
}

QTEST_APPLESS_MAIN(Rfc1036Test)
