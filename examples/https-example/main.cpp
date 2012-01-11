#include <QCoreApplication>
#include <Tufao/HttpsServer>
#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include "mainhandler.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Tufao::HttpsServer server;

    QFile key(":/key.pem");
    key.open(QIODevice::ReadOnly);
    server.setPrivateKey(QSslKey(&key, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey,
                                 "123456"));

    QFile cert(":/cert.pem");
    cert.open(QIODevice::ReadOnly);
    server.setLocalCertificate(QSslCertificate(&cert));

    MainHandler h;

    QObject::connect(&server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
                     &h, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
