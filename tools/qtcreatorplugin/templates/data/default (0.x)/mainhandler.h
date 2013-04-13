#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include <QObject>

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

} // namespace Tufao

class MainHandler : public QObject
{
    Q_OBJECT
public:
    explicit MainHandler(QObject *parent = 0);

public slots:
    void handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response);
};

#endif // MAINHANDLER_H
