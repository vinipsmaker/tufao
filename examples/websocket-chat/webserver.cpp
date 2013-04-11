/*
  Copyright (c) 2012, 2013 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

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

                    response.writeHead(Tufao::HttpResponseStatus::OK);
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
