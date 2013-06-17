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

    for (int i = 0;i != 2;++i) {
        fillCookieData(30 + i * 30, false, false);
        fillCookieData(30 + i * 30, false, true);
        fillCookieData(30 + i * 30, true, false);
        fillCookieData(30 + i * 30, true, true);
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

    // You MUST NOT compare the two values, because the cookies are generated on
    // demand and the expiration date might differ. All depends on luck.
    QNetworkCookie cookies[2] = {
        settings.cookie(value),
        SessionSettings::cookie(settings, value)
    };

    for (int i = 0;i != 2;++i) {
        const QNetworkCookie cookie = cookies[i];

        int tol = 10;
        int val = QDateTime::currentDateTimeUtc().addSecs(timeout * 60)
            .secsTo(cookie.expirationDate());
        // If the computer is very very very very slow, this might fail
        QVERIFY(val >= 0);
        QVERIFY(val < tol);

        QCOMPARE(cookie.isHttpOnly(), httpOnly);
        QCOMPARE(cookie.name(), name);
        QCOMPARE(cookie.path(), QString(path));
        QCOMPARE(cookie.isSecure(), secure);
        QCOMPARE(cookie.domain(), QString(domain));
        QCOMPARE(cookie.value(), value);
    }
}

inline void SessionSettingsTest::fillCookieData(int timeout, bool httpOnly,
                                                bool secure)
{
    QTest::newRow("bergamota and furrovine")
        << timeout << httpOnly << secure
        // name
        << QByteArray("bergamota-key")
        // path
        << QByteArray("/stairway/to/heaven")
        // domain
        << QByteArray("lemadscientist.noow")
        // value
        << QByteArray("furrovine");
    QTest::newRow("foobar")
        << timeout << httpOnly << secure
        // name
        << QByteArray("foo")
        // path
        << QByteArray("/highway/to/hell")
        // domain
        << QByteArray("thegood.die.young")
        // value
        << QByteArray("bar");
}

QTEST_APPLESS_MAIN(SessionSettingsTest)
