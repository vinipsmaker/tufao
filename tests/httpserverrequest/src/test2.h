#ifndef TEST2_H
#define TEST2_H

#include "abstracttest.h"
#include "stringlistsocket.h"
#include <QTimer>

class Test2 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test2(QObject *parent = 0);

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

#endif // TEST2_H
