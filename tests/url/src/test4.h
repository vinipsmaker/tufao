#include <Tufao/Url>

inline void test4()
{
    using namespace Tufao;
    qDebug("Running test4");
    Url u("/test#fragment");
    Q_ASSERT(u.scheme() == "");
    Q_ASSERT(u.userinfo() == "");
    Q_ASSERT(u.authority() == "");
    Q_ASSERT(u.path() == "/test");
    Q_ASSERT(u.query() == "");
    Q_ASSERT(u.fragment() == "fragment");
    Q_ASSERT(u.username() == "");
    Q_ASSERT(u.password() == "");
    Q_ASSERT(u.hostname() == "");
    Q_ASSERT(u.port() == "");
}

