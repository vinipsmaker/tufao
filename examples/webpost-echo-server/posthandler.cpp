#include "posthandler.h"

#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

using namespace Tufao;

PostHandler::PostHandler(HttpServerRequest *request,
                         HttpServerResponse *response, QObject *parent) :
    QObject(parent),
    request(request),
    response(response)
{
    connect(request, SIGNAL(data(QByteArray)), this, SLOT(onData(QByteArray)));
    connect(request, SIGNAL(end()), this, SLOT(onEnd()));
}

void PostHandler::onData(const QByteArray &chunk)
{
    buffer += chunk;
}

void PostHandler::onEnd()
{
    // handle request:
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().insert("Content-Type", "text/plain");
    response->end(buffer);
}
