#include "url.h"
#include <QtTest/QTest>
#include "../url.h"

using namespace Tufao;

void UrlTest::parse_data()
{
    QTest::addColumn<QString>("fullUrl");

    QTest::addColumn<QString>("scheme");
    QTest::addColumn<QString>("userinfo");
    QTest::addColumn<QString>("authority");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("query");
    QTest::addColumn<QString>("fragment");
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("password");
    QTest::addColumn<QString>("hostname");
    QTest::addColumn<QString>("port");

    QTest::newRow("scheme://userinfo@hostname:port/path?query#fragment")
        << "scheme://userinfo@hostname:port/path?query#fragment" << "scheme"
        << "userinfo" << "userinfo@hostname:port" << "/path" << "query"
        << "fragment" << "userinfo" << "" << "hostname" << "port";

    QTest::newRow("scheme://username:userpass@hostname/#fragment")
        << "scheme://username:userpass@hostname/#fragment" << "scheme"
        << "username:userpass" << "username:userpass@hostname" << "/" << ""
        << "fragment" << "username" << "userpass" << "hostname" << "";

    QTest::newRow("scheme://hostname/")
        << "scheme://hostname/" << "scheme" << "" << "hostname" << "/" << ""
        << "" << "" << "" << "hostname" << "";

    QTest::newRow("/test#fragment")
        << "/test#fragment" << "" << "" << "" << "/test" << "" << "fragment"
        << "" << "" << "" << "";
}

void UrlTest::parse()
{
    QFETCH(QString, fullUrl);

    QFETCH(QString, scheme);
    QFETCH(QString, userinfo);
    QFETCH(QString, authority);
    QFETCH(QString, path);
    QFETCH(QString, query);
    QFETCH(QString, fragment);
    QFETCH(QString, username);
    QFETCH(QString, password);
    QFETCH(QString, hostname);
    QFETCH(QString, port);

    Url u(fullUrl);

    QCOMPARE(u.scheme(), scheme);
    QCOMPARE(u.userinfo(), userinfo);
    QCOMPARE(u.authority(), authority);
    QCOMPARE(u.path(), path);
    QCOMPARE(u.query(), query);
    QCOMPARE(u.fragment(), fragment);
    QCOMPARE(u.username(), username);
    QCOMPARE(u.password(), password);
    QCOMPARE(u.hostname(), hostname);
    QCOMPARE(u.port(), port);
}

QTEST_APPLESS_MAIN(UrlTest)
