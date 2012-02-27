#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include <Tufao/HttpServer>

class WebServer : public Tufao::HttpServer
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = 0);

signals:
    void newMessage(QByteArray msg);

public slots:
    void handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response);

protected:
    void upgrade(Tufao::HttpServerRequest *request, const QByteArray &head);
};

#endif // MAINHANDLER_H
