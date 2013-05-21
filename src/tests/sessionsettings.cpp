#include "sessionsettings.h"
#include <QtTest/QTest>
#include "../sessionsettings.h"

using namespace Tufao;

void SessionSettingsTest::cookie_data()
{
    QTest::addColumn<int>("timeout");
    QTest::addColumn<bool>("httpOnly");
    QTest::addColumn<bool>("secure");
    QTest::addColumn<QByteArray>("name");
    QTest::addColumn<QByteArray>("path");
    QTest::addColumn<QByteArray>("domain");
    QTest::addColumn<QByteArray>("value");

    auto fill = [](int timeout, bool httpOnly, bool secure) {
        QTest::newRow("bergamota and furrovine") << timeout << httpOnly
            << secure
            // name
            << QByteArray{"bergamota-key"}
            // path
            << QByteArray{"/stairway/to/heaven"}
            // domain
            << QByteArray{"lemadscientist.noow"}
            // value
            << QByteArray{"furrovine"};
        QTest::newRow("foobar") << timeout << httpOnly
            << secure
            // name
            << QByteArray{"foo"}
            // path
            << QByteArray{"/highway/to/hell"}
            // domain
            << QByteArray{"thegood.die.young"}
            // value
            << QByteArray{"bar"};
    };

    for (int i = 0;i != 2;++i) {
        fill(30 + i * 30, false, false);
        fill(30 + i * 30, false, true);
        fill(30 + i * 30, true, false);
        fill(30 + i * 30, true, true);
    }
}

void SessionSettingsTest::cookie()
{
    QFETCH(int, timeout);
    QFETCH(bool, httpOnly);
    QFETCH(QByteArray, name);
    QFETCH(QByteArray, path);
    QFETCH(bool, secure);
    QFETCH(QByteArray, domain);
    QFETCH(QByteArray, value);

    SessionSettings settings;
    settings.timeout = timeout;
    settings.httpOnly = httpOnly;
    settings.name = name;
    settings.path = path;
    settings.secure = secure;
    settings.domain = domain;

    QCOMPARE(settings.timeout, timeout);
    QCOMPARE(settings.httpOnly, httpOnly);
    QCOMPARE(settings.name, name);
    QCOMPARE(settings.path, path);
    QCOMPARE(settings.secure, secure);
    QCOMPARE(settings.domain, domain);

    auto testExpire =  [&](const QDateTime &dateTime) {
        int tol = 10;
        auto val = QDateTime::currentDateTimeUtc().addSecs(timeout * 60)
            .secsTo(dateTime);
        // If the computer is very very very very slow, this might fail
        QVERIFY(val >= 0);
        QVERIFY(val < tol);
    };

    auto testCookie = [&](QNetworkCookie cookie) {
        testExpire(cookie.expirationDate());
        QCOMPARE(cookie.isHttpOnly(), httpOnly);
        QCOMPARE(cookie.name(), name);
        QCOMPARE(cookie.path(), QString{path});
        QCOMPARE(cookie.isSecure(), secure);
        QCOMPARE(cookie.domain(), QString{domain});
        QCOMPARE(cookie.value(), value);
    };

    // You MUST NOT compare the two values, because the cookies are generated on
    // demand and the expiration date might differ. All depends on luck.
    testCookie(settings.cookie(value));
    testCookie(SessionSettings::cookie(settings, value));
}

QTEST_APPLESS_MAIN(SessionSettingsTest)
