#include <QCoreApplication>

#include <Tufao/HttpServer>
#include <Tufao/HttpServerResponse>

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServer server;

    QObject::connect(&server, &HttpServer::requestReady,
                     [](HttpServerRequest &, HttpServerResponse &res) {
                         res.writeHead(200, "OK");
                         res.end("Hello World\n");
    });

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
