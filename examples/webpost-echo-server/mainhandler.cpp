#include "mainhandler.h"

#include "posthandler.h"

#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>
#include <Tufao/Url>

using namespace Tufao;

MainHandler::MainHandler(QObject *parent) :
    QObject(parent),
    fileServer(":/")
{
}

void MainHandler::handleRequest(HttpServerRequest *request,
                                HttpServerResponse *response)
{
    if (request->method() == "POST") {
        PostHandler *handler = new PostHandler(request, response);
        connect(response, SIGNAL(finished()), handler, SLOT(deleteLater()));
        return;
    }

    if (fileServer.handleRequest(request, response))
        return;

    if (Url(request->url()).path() == "/") {
        response->writeHead(HttpServerResponse::MOVED_PERMANENTLY);
        response->headers().insert("Location", "/index.html");
        response->end();
        return;
    }

    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().insert("Content-Type", "text/plain");
    response->end("Hello World\n");
}
