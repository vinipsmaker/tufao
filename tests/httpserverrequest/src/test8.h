#ifndef TEST8_H
#define TEST8_H

#include "abstracttest.h"
#include "stringlistsocket.h"

class Test8 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test8(QObject *parent = 0);

    void operator ()();

    void testRequest(Tufao::HttpServerRequest *request,
                     Tufao::HttpServerResponse *response);

private slots:
    void onReady();

private:
    StringListSocket *socket;
};

#endif // TEST8_H
