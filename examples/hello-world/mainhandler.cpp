#include "mainhandler.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

MainHandler::MainHandler(QObject *parent) :
    QObject(parent)
{
}

void MainHandler::handleRequest(Tufao::HttpServerRequest *request,
                                Tufao::HttpServerResponse *response)
{
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "text/plain");
    response->end("Hello " + request->url());
}
