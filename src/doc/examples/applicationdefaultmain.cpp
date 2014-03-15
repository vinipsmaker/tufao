#include <QCoreApplication>

#include <Tufao/HttpFileServer>
#include <Tufao/NotFoundHandler>

#include <Tufao/HttpServerRequestRouter>
#include <Tufao/HttpServer>

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServerRequestRouter router{
        {QRegularExpression{""}, HttpFileServer::handler("public")},
        {QRegularExpression{""}, NotFoundHandler::handler()}
    };

    HttpServer server;

    QObject::connect(&server, &HttpServer::requestReady,
                     &router, &HttpServerRequestRouter::handleRequest);

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
