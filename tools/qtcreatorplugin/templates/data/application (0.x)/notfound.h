#ifndef NOTFOUNDHANDLER_H
#define NOTFOUNDHANDLER_H

#include <Tufao/AbstractHttpServerRequestHandler>

class NotFoundHandler : public Tufao::AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    explicit NotFoundHandler(QObject *parent = 0);

public slots:
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList &args);
};

#endif // NOTFOUNDHANDLER_H
