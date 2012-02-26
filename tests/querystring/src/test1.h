#include <Tufao/QueryString>
#include <QByteArray>

/*!
  Tests a simple query string.
  */
inline void test1()
{
    using namespace Tufao;
    qDebug("Running test1");
    QByteArray q("name=tux&typ%C3%A9=peng%C3%BCin&age=20");

    QMap<QByteArray, QByteArray> map;
    map["name"] = "tux";
    map["typé"] = "pengüin";
    map["age"] = "20";

    QMap<QByteArray, QByteArray> expected = QueryString::parse(q);
    Q_ASSERT(map == expected);
}
