#include "pluginreloader.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpPluginServer>

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QHostAddress>

PluginReloader::PluginReloader(Tufao::HttpPluginServer *server,
                               QObject *parent) :
    Tufao::AbstractHttpServerRequestHandler(parent),
    server(server)
{
}

bool PluginReloader::handleRequest(Tufao::HttpServerRequest *request,
                                   Tufao::HttpServerResponse *response,
                                   const QStringList &args)
{
    Q_UNUSED(args)

    QAbstractSocket *socket = request->socket();

    if (socket->peerAddress() != QHostAddress::LocalHost)
        return false;

    server->reloadConfig();

    response->writeHead(Tufao::HttpServerResponse::OK);
    response->end("Plugins reloaded\n");

    return true;
}
