#ifndef TEST9_H
#define TEST9_H

#include "abstracttest.h"
#include "stringlistsocket.h"

class Test9 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test9(QObject *parent = 0);

    void operator ()();

    void testRequest(Tufao::HttpServerRequest *request,
                     Tufao::HttpServerResponse *response);

private slots:
    void onData(const QByteArray &chunk);
    void onEnd();
    void onFail();

private:
    StringListSocket *socket;
    Tufao::HttpServerRequest *request;
    Tufao::HttpServerResponse *response;
    QByteArray buffer;
    bool ok;
};

#endif // TEST9_H
