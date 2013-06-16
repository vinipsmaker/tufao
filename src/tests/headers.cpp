#include "headers.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include "../headers.h"

using namespace Tufao;

void HeadersTest::caseInsensitivity_data()
{
    // qbytearray1 == qbytearray2
    QTest::addColumn<QByteArray>("qbytearray1");
    QTest::addColumn<QByteArray>("qbytearray2");

    QTest::newRow("Bergamota and beRgaMotA") << QByteArray{"Bergamota"}
        << QByteArray{"beRgaMotA"};
    QTest::newRow("fURroVinE and FurrOvIne") << QByteArray{"fURroVinE"}
        << QByteArray{"FurrOvIne"};
}

void HeadersTest::caseInsensitivity()
{
    QFETCH(QByteArray, qbytearray1);
    QFETCH(QByteArray, qbytearray2);

    const IByteArray ibytearray1 = IByteArray{qbytearray1.constData()};
    const IByteArray ibytearray2 = qbytearray2;

    Headers headers;
    headers.insert(ibytearray1, qbytearray2);
    headers.insert(ibytearray2, qbytearray2);

    QCOMPARE(headers.count(ibytearray1), 2);
    QCOMPARE(headers.count(ibytearray2), 2);

    QVERIFY(headers.values(ibytearray1)[0] == headers.values(ibytearray2)[1]);

    /* ====================================================================== */

    headers.clear();
    headers.insert(ibytearray1, qbytearray2);
    headers.replace(ibytearray1, qbytearray1);
    headers.insert(ibytearray2, qbytearray2);

    QCOMPARE(headers.count(ibytearray1), 2);
    QCOMPARE(headers.count(ibytearray2), 2);

    QVERIFY(headers.values(ibytearray1)[0] != headers.values(ibytearray2)[1]);
}

void HeadersTest::dateTimeConversion_data()
{
    QTest::addColumn<QDateTime>("datetime");
    QTest::addColumn<QByteArray>("asctime"); // ANSI C's asctime() format
    QTest::addColumn<QByteArray>("rfc1036");
    QTest::addColumn<QByteArray>("rfc1123");

    QTest::newRow("RFC 2616's 3.3.1")
        << QDateTime{QDate{1994, 11, 6}, QTime{8, 49, 37}, Qt::UTC}
        << QByteArray{"Sun Nov  6 08:49:37 1994"}
        << QByteArray{"Sunday, 06-Nov-94 08:49:37 GMT"}
        << QByteArray{"Sun, 06 Nov 1994 08:49:37 GMT"};
    QTest::newRow("Another data")
        << QDateTime{QDate{1903, 12, 1}, QTime{}, Qt::UTC}
        << QByteArray{"Tue Dec  1 00:00:00 1903"}
        << QByteArray{"Tuesday, 01-Dec-03 00:00:00 GMT"}
        << QByteArray{"Tue, 01 Dec 1903 00:00:00 GMT"};
}

void HeadersTest::dateTimeConversion()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QByteArray, asctime);
    QFETCH(QByteArray, rfc1036);
    QFETCH(QByteArray, rfc1123);

    QLocale locales[]{
        {QLocale::C},
        {QLocale::Portuguese},
        {QLocale::Dutch}
    };

    for (auto locale: locales) {
        QLocale::setDefault(locale);

        QCOMPARE(Headers::toDateTime(asctime), datetime);
        QCOMPARE(Headers::toDateTime(rfc1036), datetime);
        QCOMPARE(Headers::toDateTime(rfc1123), datetime);
        QCOMPARE(QString{Headers::fromDateTime(datetime)}, QString{rfc1123});
    }
}

QTEST_APPLESS_MAIN(HeadersTest)
