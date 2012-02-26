#include <Tufao/QueryString>
#include <QByteArray>

inline void test4()
{
    using namespace Tufao;
    qDebug("Running test4");
    QByteArray q("age=20&name=tux&typ%C3%A9=peng%C3%BCin");

    QMap<QByteArray, QByteArray> map;
    map["name"] = "tux";
    map["typé"] = "pengüin";
    map["age"] = "20";

    QByteArray expected = QueryString::stringify(map);
    Q_ASSERT(q == expected);
}

