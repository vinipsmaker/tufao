// NOTE:
// If you want HTTPS support, uncomment all code

#include <QtCore/QCoreApplication>
//#include <QtCore/QFile>
//#include <QtNetwork/QSslCertificate>
//#include <QtNetwork/QSslKey>

#include <Tufao/HttpServer>
//#include <Tufao/HttpsServer>

#include <Tufao/HttpServerRequestRouter>
#include <Tufao/HttpFileServer>
#include <Tufao/HttpPluginServer>

#include "notfound.h"
#include "pluginreloader.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(static);
    // First we create a event loop and the server objects
    QCoreApplication a(argc, argv);
    Tufao::HttpServer server;
//    Tufao::HttpsServer secureServer;

//    {
//        QFile keyFile(":/key.pem");
//        key.open(QIODevice::ReadOnly);
//        QSslKey key(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey,
//                    "password");
//        secureServer.setPrivateKey(key);

//        QFile cert(":/cert.pem");
//        cert.open(QIODevice::ReadOnly);
//        secureServer.setLocalCertificate(QSslCertificate(&cert));
//    }

    // Then we create a request router and configure a request router
    Tufao::HttpServerRequestRouter router;
    Tufao::HttpPluginServer pluginServer("routes.conf");
    PluginReloader pluginReloader(&pluginServer);
    Tufao::HttpFileServer fileServer("public");
    NotFoundHandler handler404;

    router.map(QRegExp(""), &pluginServer)
            .map(QRegExp("^/reload$"), &pluginReloader)
            .map(QRegExp(""), &fileServer)
            .map(QRegExp(""), &handler404);

    // We set the router as global request handler
    QObject::connect(&server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
                     &router, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));
//    QObject::connect(&secureServer, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
//                     &router, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

    // Last, we run our servers
    server.listen(QHostAddress::Any, 8080);
//    secureServer.listen(QHostAddress::Any, 443);

    return a.exec();
}
