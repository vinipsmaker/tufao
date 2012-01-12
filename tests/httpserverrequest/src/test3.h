#ifndef TEST3_H
#define TEST3_H

#include "abstracttest.h"
#include "stringlistsocket.h"
#include <QTimer>

class Test3 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test3(QObject *parent = 0);

    void operator ()();

    void testRequest(Tufao::HttpServerRequest *request,
                     Tufao::HttpServerResponse *response);

private slots:
    void onDisconnected();
    void onTimer();

private:
    StringListSocket *socket;
    QTimer timer;
};

#endif // TEST3_H
