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
        res << "Responding from a evil plugin\n";
        res.end();
        return true;
    };
}
