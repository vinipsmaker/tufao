#include "priv/httpsconnectionhandler_p.h"
#include <QtNetwork/QSslSocket>

namespace Tufao {

HttpsConnectionHandler::HttpsConnectionHandler(QObject *parent) :
    HttpConnectionHandler(new Priv(),parent)
{

}

void HttpsConnectionHandler::incomingConnection(qintptr socketDescriptor)
{
    Priv* p = ((Priv*)_priv());

    QSslSocket *socket = new QSslSocket;
    socket->setProtocol(QSsl::TlsV1_0);
    socket->setLocalCertificate(p->localCertificate);
    socket->setPrivateKey(p->privateKey);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    socket->startServerEncryption();
    handleConnection(socket);
}

void HttpsConnectionHandler::setLocalCertificate(const QSslCertificate &certificate)
{
    ((Priv*)_priv())->localCertificate = certificate;
}

void HttpsConnectionHandler::setPrivateKey(const QSslKey &key)
{
    ((Priv*)_priv())->privateKey = key;
}


} // namespace Tufao
