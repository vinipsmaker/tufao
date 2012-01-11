#include "mainhandler.h"
#include <tufao/httpserverrequest.h>
#include <tufao/headers.h>

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
