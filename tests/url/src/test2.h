#include <Tufao/Url>

inline void test2()
{
    using namespace Tufao;
    qDebug("Running test2");
    Url u("scheme://username:userpass@hostname/#fragment");
    Q_ASSERT(u.scheme() == "scheme");
    Q_ASSERT(u.userinfo() == "username:userpass");
    Q_ASSERT(u.authority() == "username:userpass@hostname");
    Q_ASSERT(u.path() == "/");
    Q_ASSERT(u.query() == "");
    Q_ASSERT(u.fragment() == "fragment");
    Q_ASSERT(u.username() == "username");
    Q_ASSERT(u.password() == "userpass");
    Q_ASSERT(u.hostname() == "hostname");
    Q_ASSERT(u.port() == "");
}

