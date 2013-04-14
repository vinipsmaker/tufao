#include "plugin.h"
#include <QtCore/QtPlugin>
#include <Tufao/HttpServerResponse>

using namespace Tufao;

std::function<bool(HttpServerRequest&, HttpServerResponse&)>
Plugin::createHandler(const QHash<QString, HttpServerPlugin*> &,
                      const QVariant &)
{
    return [](HttpServerRequest &, HttpServerResponse &res){
        res.writeHead(HttpResponseStatus::OK);
        res.end("Hello World\n");
        return true;
    };
}
