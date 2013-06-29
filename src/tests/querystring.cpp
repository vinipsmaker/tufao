#include "querystring.h"
#include <QtTest/QTest>
#include "../querystring.h"

using namespace Tufao;
typedef QMap<QByteArray, QByteArray> MapType;
Q_DECLARE_METATYPE(MapType)

void QueryStringTest::parse_data()
{
    // QueryString::parse(querystring) == map
    QTest::addColumn<QByteArray>("querystring");
    QTest::addColumn<MapType>("map");

    {
        QMap<QByteArray, QByteArray> map;
        map["name"] = "tux";
        map["typé"] = "pengüin";
        map["age"] = "20";

        // Tests a simple query string
        QTest::newRow("name=tux&typ%C3%A9=peng%C3%BCin&age=20")
            << QByteArray("name=tux&typ%C3%A9=peng%C3%BCin&age=20") << map;
    }
    {
        QMap<QByteArray, QByteArray> map;
        map["name"] = "tux";
        map["type"] = "penguin";
        map["age"] = "20";

        // Tests a query string with extra separator characters
        QTest::newRow("&&&name=tux&&&type=penguin&age=20&&")
            << QByteArray("&&&name=tux&&&type=penguin&age=20&&") << map;
    }
    {
        QMap<QByteArray, QByteArray> map;
        map["name"] = "tux";
        map["type"];
        map["age"];

        // Tests a query string with an empty value
        QTest::newRow("name=tux&type&age=")
            << QByteArray("name=tux&type&age=") << map;
    }
}

void QueryStringTest::parse()
{
    QFETCH(QByteArray, querystring);
    QFETCH(MapType, map);

    QCOMPARE(QueryString::parse(querystring), map);
}

void QueryStringTest::stringify_data()
{
    // querystring == QueryString::stringify(map)
    QTest::addColumn<QByteArray>("querystring");
    QTest::addColumn< MapType >("map");

    {
        QMap<QByteArray, QByteArray> map;
        map["name"] = "tux";
        map["typé"] = "pengüin";
        map["age"] = "20";

        QTest::newRow("age=20&name=tux&typ%C3%A9=peng%C3%BCin")
            << QByteArray("age=20&name=tux&typ%C3%A9=peng%C3%BCin") << map;
    }
}

void QueryStringTest::stringify()
{
    QFETCH(QByteArray, querystring);
    QFETCH(MapType, map);

    QCOMPARE(QueryString::stringify(map), querystring);
}

QTEST_APPLESS_MAIN(QueryStringTest)
