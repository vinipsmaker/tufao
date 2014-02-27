#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include <Tufao/HttpServer>

class WebServer : public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = 0);

signals:
    void newMessage(QByteArray msg);

private:
    Tufao::HttpServer server;
};

#endif // MAINHANDLER_H
