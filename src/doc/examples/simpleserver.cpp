#include <Tufao/HttpServer>

class WebServer: public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = NULL) :
        QObject(parent),
        httpServer(new Tufao::HttpServer(this))
    {
        using namespace Tufao;
        connect(httpServer, &HttpServer::requestReady,
                [](HttpServerRequest &request, HttpServerResponse &response) {
                    response.writeHead(200, "OK");
                    response.headers().insert("Content-Type", "text/plain");
                    response.write("Hello World\n");
                    response.end();
        });

        httpServer->listen(QHostAddress::Any, 8080);
    }

private:
    Tufao::HttpServer *httpServer;
};
