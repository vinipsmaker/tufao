#include "requesthandler.h"
#include <Tufao/HttpServerRequest>

RequestHandler::RequestHandler(QObject *parent) :
    Tufao::AbstractHttpServerRequestHandler(parent)
{
}

bool RequestHandler::handleRequest(Tufao::HttpServerRequest *request,
                                   Tufao::HttpServerResponse *response,
                                   const QStringList &args)
{
    response->writeHead(200);
    response->end("Hello World\n");
    return true;
}
