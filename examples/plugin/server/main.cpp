#include <QtCore/QCoreApplication>

#include <Tufao/HttpServer>

#include <Tufao/HttpServerRequestRouter>
#include <Tufao/HttpPluginServer>

#include "notfound.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(static);
    QCoreApplication a(argc, argv);
    Tufao::HttpServer server;

    Tufao::HttpServerRequestRouter router;

    Tufao::HttpPluginServer pluginServer("routes.conf");
    NotFoundHandler handler404;

    router.map(QRegExp(""), &pluginServer)
            .map(QRegExp(""), &handler404);

    QObject::connect(&server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
                     &router, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
