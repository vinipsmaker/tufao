#include "cryptography.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include "../priv/cryptography.h"

void CryptographyTest::hmacSha1_data()
{
    QTest::addColumn<QByteArray>("key");
    QTest::addColumn<QByteArray>("message");
    QTest::addColumn<QByteArray>("encrypted");

    QTest::newRow("Empty HMAC values")
        << QByteArray{""}
        << QByteArray{""}
        << QByteArray{"\xFB\xDB\x1D\x1B\x18\xAA\x6C\x08\x32\x4B"
                      "\x7D\x64\xB7\x1F\xB7\x63\x70\x69\x0E\x1D"}.toBase64();

    QTest::newRow("Wikipedia example")
        << QByteArray{"key"}
        << QByteArray{"The quick brown fox jumps over the lazy dog"}
        << QByteArray{"\xDE\x7C\x9B\x85\xB8\xB7\x8A\xA6\xBC\x8A"
                      "\x7A\x36\xF7\x0A\x90\x70\x1C\x9D\xB4\xD9"}.toBase64();
}

void CryptographyTest::hmacSha1()
{
    QFETCH(QByteArray, key);
    QFETCH(QByteArray, message);
    QFETCH(QByteArray, encrypted);

    QCOMPARE(Tufao::hmacSha1(key, message), encrypted);
}

QTEST_APPLESS_MAIN(CryptographyTest)
