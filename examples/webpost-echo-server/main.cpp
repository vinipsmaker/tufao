#include <QCoreApplication>
#include <QUrl>
#include <Tufao/HttpServer>
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpFileServer>

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    HttpServer server;
    HttpFileServer fileServer(":/");

    QObject::connect(&server, &HttpServer::requestReady,
                     [&fileServer](HttpServerRequest &req,
                     HttpServerResponse &res) {
        if (req.method() == "POST") {
            QObject::connect(&req, &HttpServerRequest::end, [&req,&res]() {
                res.writeHead(Tufao::HttpResponseStatusCode::OK);
                res.headers().insert("Content-Type", "text/plain");
                res.end(req.readBody());
            });
            return;
        }

        if (fileServer.handleRequest(req, res))
            return;

        res.writeHead(HttpResponseStatusCode::MOVED_PERMANENTLY);
        res.headers().insert("Location", "/index.html");
        res.end();
    });

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
