#include "webserver.h"

#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>
#include <Tufao/WebSocket>

#include <QFile>

WebServer::WebServer(QObject *parent) :
    Tufao::HttpServer(parent)
{
    connect(this, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
            this, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));
}

void WebServer::handleRequest(Tufao::HttpServerRequest *request,
                                Tufao::HttpServerResponse *response)
{
    if (request->url() == "/"
            || request->url() == "/index.html") {
        QFile indexFile(":/index.html");
        indexFile.open(QIODevice::ReadOnly);

        response->writeHead(Tufao::HttpServerResponse::OK);
        response->headers().insert("Content-Type", "text/html; charset=utf8");
        response->end(indexFile.readAll());
    } else {
        response->writeHead(404);
        response->end("Not found");
    }
}

void WebServer::upgrade(Tufao::HttpServerRequest *request,
                          const QByteArray &head)
{
    if (request->url() != "/chat") {
        Tufao::HttpServerResponse response(request->socket(),
                                           request->responseOptions());
        response.writeHead(404);
        response.end("Not found");
        request->socket()->close();
        return;
    }

    Tufao::WebSocket *socket = new Tufao::WebSocket(this);
    socket->startServerHandshake(request, head);
    socket->setMessagesType(Tufao::WebSocket::TEXT_MESSAGE);

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(newMessage(QByteArray)),
            this, SIGNAL(newMessage(QByteArray)));
    connect(this, SIGNAL(newMessage(QByteArray)),
            socket, SLOT(sendMessage(QByteArray)));
}
