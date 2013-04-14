#include <QCoreApplication>

#include <Tufao/HttpServer>
#include <Tufao/HttpServerRequest>

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServer server;

    QObject::connect(&server, &HttpServer::requestReady,
                     [](HttpServerRequest &, HttpServerResponse &res){
                         res.writeHead(HttpResponseStatus::OK);
                         res.end("Hello World");
                     });

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
