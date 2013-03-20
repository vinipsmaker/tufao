#include <QtCore/QCoreApplication>

#include <Tufao/HttpServer>

#include <Tufao/HttpServerRequestRouter>
#include <Tufao/HttpPluginServer>

#include "notfound.h"
#include "pluginreloader.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(static);
    QCoreApplication a(argc, argv);
    Tufao::HttpServer server;

    Tufao::HttpServerRequestRouter router;

    Tufao::HttpPluginServer pluginServer("routes.conf");
    PluginReloader pluginReloader(&pluginServer);
    NotFoundHandler handler404;

    router.map(QRegExp(""), &pluginServer)
            .map(QRegExp("^/reload$"), &pluginReloader)
            .map(QRegExp(""), &handler404);

    QObject::connect(&server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
                     &router, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
