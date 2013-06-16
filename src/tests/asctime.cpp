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
        << QDateTime{QDate{1994, 11, 6}, QTime{8, 49, 37}, Qt::UTC}
        << QByteArray{"Sun Nov  6 08:49:37 1994"};
    QTest::newRow("Another data")
        << QDateTime{QDate{1903, 12, 1}, QTime{}, Qt::UTC}
        << QByteArray{"Tue Dec  1 00:00:00 1903"};
}

void AsctimeTest::dateTimeConversion()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QByteArray, asctime);

    QLocale locales[]{
        {QLocale::C},
        {QLocale::Portuguese},
        {QLocale::Dutch}
    };

    for (auto locale: locales) {
        QLocale::setDefault(locale);

        QVERIFY(static_cast<bool>(Asctime{asctime}));
        QCOMPARE(Asctime{asctime}(), datetime);
    }
}

QTEST_APPLESS_MAIN(AsctimeTest)
