#include "httpserverrequestrouter.h"
#include <QtTest/QTest>
#include <QtCore/QSharedPointer>
#include "../httpserverrequestrouter.h"

using namespace Tufao;

struct Helper
{
    Helper(bool &b): b(b) {}
    ~Helper() {b = false;}
    bool &b;
};

void HttpServerRequestRouterTest::mappings()
{
    Q_DECL_CONSTEXPR auto size = 3;
    bool active[size] = {true, true, true};

    HttpServerRequestRouter httpServerRequestRouter;

    for (int i = 0;i != size;++i) {
        QSharedPointer<Helper> p{new Helper{active[i]}};
        QCOMPARE(httpServerRequestRouter.map({
                    QRegularExpression{},
                    [p](HttpServerRequest&, HttpServerResponse&){return true;}
                }), i);
    }

    QCOMPARE(active[0], true);
    QCOMPARE(active[1], true);
    QCOMPARE(active[2], true);

    httpServerRequestRouter.unmap(1);
    QCOMPARE(active[0], true);
    QCOMPARE(active[1], false);
    QCOMPARE(active[2], true);
    QCOMPARE(httpServerRequestRouter.map({
                QRegularExpression{},
                [](HttpServerRequest&, HttpServerResponse&){return true;}
            }), 2);

    httpServerRequestRouter.unmap(1);
    QCOMPARE(active[0], true);
    QCOMPARE(active[1], false);
    QCOMPARE(active[2], false);

    httpServerRequestRouter.unmap(0);
    QCOMPARE(active[0], false);
    QCOMPARE(active[1], false);
    QCOMPARE(active[2], false);
    QCOMPARE(httpServerRequestRouter.map({
                QRegularExpression{},
                [](HttpServerRequest&, HttpServerResponse&){return true;}
            }), 1);
}

QTEST_APPLESS_MAIN(HttpServerRequestRouterTest)
