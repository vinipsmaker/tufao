#include <Tufao/QueryString>
#include <QByteArray>

/*!
  Tests a query string with an empty value
  */
inline void test3()
{
    using namespace Tufao;
    qDebug("Running test3");
    QByteArray q("name=tux&type&age=");

    QMap<QByteArray, QByteArray> map;
    map["name"] = "tux";
    map["type"];
    map["age"];

    QMap<QByteArray, QByteArray> expected = QueryString::parse(q);
    Q_ASSERT(map == expected);
}

