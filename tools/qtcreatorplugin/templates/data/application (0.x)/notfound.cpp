#include "notfound.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpFileServer>

NotFoundHandler::NotFoundHandler(QObject *parent) :
    Tufao::AbstractHttpServerRequestHandler(parent)
{
}

bool NotFoundHandler::handleRequest(Tufao::HttpServerRequest *request,
                                    Tufao::HttpServerResponse *response,
                                    const QStringList &args)
{
    Q_UNUSED(request)
    Q_UNUSED(args)

    Tufao::HttpFileServer::serveFile(":/notfound.html", response,
                                     Tufao::HttpServerResponse::NOT_FOUND);

    // Always handles the requests
    return true;
}
