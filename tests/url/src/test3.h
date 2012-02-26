#include <Tufao/Url>

inline void test3()
{
    using namespace Tufao;
    qDebug("Running test3");
    Url u("scheme://hostname/");
    Q_ASSERT(u.scheme() == "scheme");
    Q_ASSERT(u.userinfo() == "");
    Q_ASSERT(u.authority() == "hostname");
    Q_ASSERT(u.path() == "/");
    Q_ASSERT(u.query() == "");
    Q_ASSERT(u.fragment() == "");
    Q_ASSERT(u.username() == "");
    Q_ASSERT(u.password() == "");
    Q_ASSERT(u.hostname() == "hostname");
    Q_ASSERT(u.port() == "");
}

