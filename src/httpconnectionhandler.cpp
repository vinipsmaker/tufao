#include "priv/httpconnectionhandler_p.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

namespace Tufao {

inline HttpConnectionHandler::Priv::Priv() :
    upgradeHandler(defaultUpgradeHandler)
{}

HttpConnectionHandler::UpgradeHandler HttpConnectionHandler::Priv::defaultUpgradeHandler{
    [](HttpServerRequest &request, const QByteArray&){
        request.socket().close();
    }
};

HttpConnectionHandler::HttpConnectionHandler(QObject *parent) :
    HttpConnectionHandler(new HttpConnectionHandler::Priv(),parent)
{
}

HttpConnectionHandler::HttpConnectionHandler(HttpConnectionHandler::Priv *p, QObject *parent)
    : AbstractConnectionHandler(p,parent)
{

}

void HttpConnectionHandler::setUpgradeHandler(HttpConnectionHandler::UpgradeHandler functor)
{
    if (!functor)
        return;

    ((Priv*)_priv())->upgradeHandler = functor;
}

bool HttpConnectionHandler::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket;

    qDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Incoming Connection");

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return false;
    }

    handleConnection(socket);
    return true;
}

void HttpConnectionHandler::checkContinue(HttpServerRequest &request, HttpServerResponse &response)
{
    response.writeContinue();
    emit requestReady(request, response);
}

void HttpConnectionHandler::handleUpgrade(HttpServerRequest &request)
{
    ((Priv*)_priv())->upgradeHandler(request,request.readBody());
}

HttpConnectionHandler::UpgradeHandler HttpConnectionHandler::defaultUpgradeHandler()
{
    return Priv::defaultUpgradeHandler;
}

} // namespace Tufao


void Tufao::HttpConnectionHandler::closePendingConnection(qintptr socketDescriptor)
{
    QTcpSocket sock;
    sock.setSocketDescriptor(socketDescriptor);

    //TODO write some status

    sock.close();
}
