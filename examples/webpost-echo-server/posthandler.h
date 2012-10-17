#ifndef POSTHANDLER_H
#define POSTHANDLER_H

#include <QObject>

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

} // namespace Tufao

class PostHandler : public QObject
{
    Q_OBJECT
public:
    explicit PostHandler(Tufao::HttpServerRequest *request,
                         Tufao::HttpServerResponse *response,
                         QObject *parent = 0);

private slots:
    void onData(const QByteArray &chunk);
    void onEnd();

private:
    Tufao::HttpServerRequest *request;
    Tufao::HttpServerResponse *response;
    QByteArray buffer;
};

#endif // POSTHANDLER_H
