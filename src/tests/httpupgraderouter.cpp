#include "httpupgraderouter.h"
#include <QtTest/QTest>
#include <QtCore/QSharedPointer>
#include "../httpupgraderouter.h"

using namespace Tufao;

struct Helper
{
    Helper(bool &b): b(b) {}
    ~Helper() {b = false;}
    bool &b;
};

void HttpUpgradeRouterTest::mappings()
{
    Q_DECL_CONSTEXPR auto size = 3;
    bool active[size] = {true, true, true};

    HttpUpgradeRouter httpUpgradeRouter;

    for (int i = 0;i != size;++i) {
        QSharedPointer<Helper> p{new Helper{active[i]}};
        QCOMPARE(httpUpgradeRouter.map({
                    QRegularExpression{},
                    [p](HttpServerRequest&, const QByteArray &){return true;}
                }), i);
    }

    QCOMPARE(active[0], true);
    QCOMPARE(active[1], true);
    QCOMPARE(active[2], true);

    httpUpgradeRouter.unmap(1);
    QCOMPARE(active[0], true);
    QCOMPARE(active[1], false);
    QCOMPARE(active[2], true);
    QCOMPARE(httpUpgradeRouter.map({
                QRegularExpression{},
                [](HttpServerRequest&, const QByteArray &){return true;}
            }), 2);

    httpUpgradeRouter.unmap(1);
    QCOMPARE(active[0], true);
    QCOMPARE(active[1], false);
    QCOMPARE(active[2], false);

    httpUpgradeRouter.unmap(0);
    QCOMPARE(active[0], false);
    QCOMPARE(active[1], false);
    QCOMPARE(active[2], false);
    QCOMPARE(httpUpgradeRouter.map({
                QRegularExpression{},
                [](HttpServerRequest&, const QByteArray &){return true;}
            }), 1);
}

QTEST_APPLESS_MAIN(HttpUpgradeRouterTest)
