#ifndef TEST1_H
#define TEST1_H

#include "abstracttest.h"
#include "stringlistsocket.h"

class Test1 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test1(QObject *parent = 0);

    void operator ()();

    void testRequest(Tufao::HttpServerRequest *request,
                     Tufao::HttpServerResponse *response);

private slots:
    void onReady();

private:
    StringListSocket *socket;
};

#endif // TEST1_H
