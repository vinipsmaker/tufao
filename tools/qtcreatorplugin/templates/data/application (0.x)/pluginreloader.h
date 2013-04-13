#ifndef PLUGINRELOADER_H
#define PLUGINRELOADER_H

#include <Tufao/AbstractHttpServerRequestHandler>

namespace Tufao {
class HttpPluginServer;
}

class PluginReloader : public Tufao::AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    explicit PluginReloader(Tufao::HttpPluginServer *server,
                            QObject *parent = 0);

public slots:
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList &args);

private:
    Tufao::HttpPluginServer *server;
};

#endif // PLUGINRELOADER_H
