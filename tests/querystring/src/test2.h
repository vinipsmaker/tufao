#include <Tufao/QueryString>
#include <QByteArray>

/*!
  Tests a query string with extra separator characters.
  */
inline void test2()
{
    using namespace Tufao;
    qDebug("Running test2");
    QByteArray q("&&&name=tux&&&type=penguin&age=20&&");

    QMap<QByteArray, QByteArray> map;
    map["name"] = "tux";
    map["type"] = "penguin";
    map["age"] = "20";

    QMap<QByteArray, QByteArray> expected = QueryString::parse(q);
    Q_ASSERT(map == expected);
}

