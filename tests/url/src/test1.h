#include <Tufao/Url>

inline void test1()
{
    using namespace Tufao;
    qDebug("Running test1");
    Url u("scheme://userinfo@hostname:port/path?query#fragment");
    Q_ASSERT(u.scheme() == "scheme");
    Q_ASSERT(u.userinfo() == "userinfo");
    Q_ASSERT(u.authority() == "userinfo@hostname:port");
    Q_ASSERT(u.path() == "/path");
    Q_ASSERT(u.query() == "query");
    Q_ASSERT(u.fragment() == "fragment");
    Q_ASSERT(u.username() == "userinfo");
    Q_ASSERT(u.password() == "");
    Q_ASSERT(u.hostname() == "hostname");
    Q_ASSERT(u.port() == "port");
}
