#ifndef TEST5_H
#define TEST5_H

#include "abstracttest.h"
#include "stringlistsocket.h"
#include <QTimer>

class Test5 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test5(QObject *parent = 0);

    void operator ()();

    void testRequest(Tufao::HttpServerRequest *request,
                     Tufao::HttpServerResponse *response);

private slots:
    void onDisconnected();
    void onTimer();

private:
    StringListSocket *socket;
    QTimer timer;
    bool requested;
};

#endif // TEST5_H
