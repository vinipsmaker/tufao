#include "httppluginserver.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include "../../httppluginserver.h"

using namespace Tufao;

void HttpPluginServerTest::config()
{
    QString nonExistingFile{":/non/existing/file"};
    QString validConfig{":/sample.json"};

    QCOMPARE(HttpPluginServer{}.config(), QString());

    QCOMPARE(HttpPluginServer{nonExistingFile}.config(), QString());
    {
        HttpPluginServer httpPluginServer;
        QCOMPARE(httpPluginServer.setConfig(nonExistingFile), false);
        QCOMPARE(httpPluginServer.config(), QString());
    }

    QCOMPARE(HttpPluginServer{validConfig}.config(), validConfig);
    {
        HttpPluginServer httpPluginServer;
        QCOMPARE(httpPluginServer.setConfig(validConfig), true);
        QCOMPARE(httpPluginServer.config(), validConfig);
    }
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(config);
    QCoreApplication app{argc, argv};
    Q_UNUSED(app);

    HttpPluginServerTest httpPluginServerTest;

    return QTest::qExec(&httpPluginServerTest, argc, argv);
}
