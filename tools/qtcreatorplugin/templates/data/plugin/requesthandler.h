#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <Tufao/AbstractHttpServerRequestHandler>

class RequestHandler: public Tufao::AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    explicit RequestHandler(QObject *parent = 0);

public slots:
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList &args = QStringList());
};

#endif // REQUESTHANDLER_H
