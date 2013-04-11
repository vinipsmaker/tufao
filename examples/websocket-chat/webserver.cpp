#include "webserver.h"

#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>
#include <Tufao/WebSocket>

#include <QFile>
#include <QUrl>

WebServer::WebServer(QObject *parent) :
    QObject(parent)
{
    connect(&server, &Tufao::HttpServer::requestReady,
            [](Tufao::HttpServerRequest &request,
               Tufao::HttpServerResponse &response) {
                if (request.url().path() == "/"
                    || request.url().path() == "/index.html") {
                    QFile indexFile(":/index.html");
                    indexFile.open(QIODevice::ReadOnly);

                    response.writeHead(Tufao::HttpResponseStatusCode::OK);
                    response.headers().insert("Content-Type",
                                              "text/html; charset=utf8");
                    response.end(indexFile.readAll());
                } else {
                    response.writeHead(404, "NOT FOUND");
                    response.end("Not found");
                }
            });

    server.setUpgradeHandler([this](Tufao::HttpServerRequest &request,
                                const QByteArray &head) {
        if (request.url().path() != "/chat") {
            Tufao::HttpServerResponse response(request.socket(),
                                               request.responseOptions());
            response.writeHead(404, "NOT FOUND");
            response.end("Not found");
            request.socket().close();
            return;
        }

        Tufao::WebSocket *socket = new Tufao::WebSocket(this);
        socket->startServerHandshake(request, head);
        socket->setMessagesType(Tufao::WebSocketMessageType::TEXT_MESSAGE);

        connect(socket, &Tufao::AbstractMessageSocket::disconnected,
                socket, &QObject::deleteLater);

        connect(socket, &Tufao::AbstractMessageSocket::newMessage,
                this, &WebServer::newMessage);
        connect(this, &WebServer::newMessage,
                socket, &Tufao::AbstractMessageSocket::sendMessage);
    });

    server.listen(QHostAddress::Any, 8080);
}
