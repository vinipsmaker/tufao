#ifndef TEST10_H
#define TEST10_H

#include "abstracttest.h"
#include "stringlistsocket.h"

class Test10 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test10(QObject *parent = 0);

    void operator ()();

    void testRequest(Tufao::HttpServerRequest *request,
                     Tufao::HttpServerResponse *response);

private:
    StringListSocket *socket;
    bool firstRequest;
};

#endif // TEST10_H
