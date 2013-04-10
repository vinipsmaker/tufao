int main(int argc, char *argv[])
{
    using namespace Tufao;

    QCoreApplication a(argc, argv);

    // ...

    HttpUpgradeRouter upgradeRouter{
        {QRegularExpression{"^/chat$"}, [](HttpServerRequest &request,
                                           const QByteArray &head) -> bool {
                WebSocket *ws = new WebSocket;

                ws->startServerHandshake(request, head);

                // ...
            }
        }
    };

    HttpServer server;
    server.setUpgradeHandler(upgradeRouter);

    QObject::connect(&server, &HttpServer::requestReady,
                     &router, &HttpServerRequestRouter::handleRequest);

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
