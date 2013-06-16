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
        << QDateTime{QDate{1994, 11, 6}, QTime{8, 49, 37}, Qt::UTC}
        << QByteArray{"Sunday, 06-Nov-94 08:49:37 GMT"};
    QTest::newRow("Another data")
        << QDateTime{QDate{1903, 12, 1}, QTime{}, Qt::UTC}
        << QByteArray{"Tuesday, 01-Dec-03 00:00:00 GMT"};
}

void Rfc1036Test::dateTimeConversion()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QByteArray, rfc1036);

    QLocale locales[]{
        {QLocale::C},
        {QLocale::Portuguese},
        {QLocale::Dutch}
    };

    for (auto locale: locales) {
        QLocale::setDefault(locale);

        QVERIFY(static_cast<bool>(Rfc1036{rfc1036}));
        QCOMPARE(Rfc1036{rfc1036}(), datetime);
    }
}

QTEST_APPLESS_MAIN(Rfc1036Test)
