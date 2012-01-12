#ifndef TEST6_H
#define TEST6_H

#include "abstracttest.h"
#include "stringlistsocket.h"
#include <QTimer>

class Test6 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test6(QObject *parent = 0);

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

#endif // TEST6_H
