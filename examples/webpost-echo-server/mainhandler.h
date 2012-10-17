#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include <QObject>
#include <Tufao/HttpFileServer>

class MainHandler : public QObject
{
    Q_OBJECT
public:
    explicit MainHandler(QObject *parent = 0);

public slots:
    void handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response);

private:
    Tufao::HttpFileServer fileServer;
};

#endif // MAINHANDLER_H
