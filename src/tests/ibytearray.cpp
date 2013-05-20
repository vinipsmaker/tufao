#include "ibytearray.h"
#include <QtTest/QTest>
#include "../ibytearray.h"

using namespace Tufao;

void IByteArrayTest::relational_data()
{
    // qbytearray1 < qbytearray2
    QTest::addColumn<QByteArray>("qbytearray1");
    QTest::addColumn<QByteArray>("qbytearray2");

    QTest::newRow("bergamota and furrovine") << QByteArray{"bergamota"}
        << QByteArray{"furrovine"};
    QTest::newRow("Bergamota and furrovine") << QByteArray{"Bergamota"}
        << QByteArray{"furrovine"};
    QTest::newRow("bergamota and Furrovine") << QByteArray{"bergamota"}
        << QByteArray{"Furrovine"};
    QTest::newRow("Bergamota and Furrovine") << QByteArray{"Bergamota"}
        << QByteArray{"Furrovine"};
}

void IByteArrayTest::relational()
{
    QFETCH(QByteArray, qbytearray1);
    QFETCH(QByteArray, qbytearray2);

    const IByteArray ibytearray1 = IByteArray{qbytearray1.constData()};
    const IByteArray ibytearray2 = qbytearray2;

    QVERIFY(ibytearray1 == ibytearray1);
    QVERIFY(!(ibytearray1 != ibytearray1));
    QVERIFY(!(ibytearray1 == ibytearray2));
    QVERIFY(ibytearray1 != ibytearray2);

    QVERIFY(ibytearray1 < ibytearray2);
    QVERIFY(!(ibytearray1 < ibytearray1));
    QVERIFY(!(ibytearray2 < ibytearray1));

    QVERIFY(!(ibytearray1 > ibytearray2));
    QVERIFY(!(ibytearray2 > ibytearray2));
    QVERIFY(ibytearray2 > ibytearray1);

    QVERIFY(ibytearray1 <= ibytearray2);
    QVERIFY(ibytearray1 <= ibytearray1);
    QVERIFY(!(ibytearray2 <= ibytearray1));

    QVERIFY(!(ibytearray1 >= ibytearray2));
    QVERIFY(ibytearray2 >= ibytearray2);
    QVERIFY(ibytearray2 >= ibytearray1);
}

void IByteArrayTest::caseInsensitivity_data()
{
    // qbytearray1 == qbytearray2
    QTest::addColumn<QByteArray>("qbytearray1");
    QTest::addColumn<QByteArray>("qbytearray2");

    QTest::newRow("Bergamota and beRgaMotA") << QByteArray{"Bergamota"}
        << QByteArray{"beRgaMotA"};
    QTest::newRow("fURroVinE and FurrOvIne") << QByteArray{"fURroVinE"}
        << QByteArray{"FurrOvIne"};
}

void IByteArrayTest::caseInsensitivity()
{
    QFETCH(QByteArray, qbytearray1);
    QFETCH(QByteArray, qbytearray2);

    const IByteArray ibytearray1 = IByteArray{qbytearray1.constData()};
    const IByteArray ibytearray2 = qbytearray2;

    QVERIFY(ibytearray1 == ibytearray2);
    QVERIFY(ibytearray1 == ibytearray1);
    QVERIFY(!(ibytearray1 != ibytearray2));
    QVERIFY(!(ibytearray2 != ibytearray2));
}

QTEST_APPLESS_MAIN(IByteArrayTest)
