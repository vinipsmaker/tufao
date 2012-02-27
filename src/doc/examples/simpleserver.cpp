#include <Tufao/HttpServer>

class WebServer: public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = NULL) :
        QObject(parent),
        httpServer(new Tufao::HttpServer(this))
    {
        connect(httpServer,
                SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*),
                this,
                SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));
        httpServer->listen(QHostAddress::Any, 8080);
    }

private slots:
    void handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response)
    {
        response->writeHead(200);
        response->setHeader("Content-Type", "text/plain");
        response->write("Hello World\n");
        response->end();
    }

private:
    Tufao::HttpServer *httpServer;
};
