#include <QCoreApplication>
#include <httpserver.h>
#include "testsguide.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Tufao::HttpServer server;
    server.listen(QHostAddress::Any, 8080);

    TestsGuide t;

    QObject::connect(&server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
                     &t, SLOT(onRequestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

    return a.exec();
}
