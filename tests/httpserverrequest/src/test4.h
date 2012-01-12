#ifndef TEST4_H
#define TEST4_H

#include "abstracttest.h"
#include "stringlistsocket.h"
#include <QTimer>

class Test4 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test4(QObject *parent = 0);

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

#endif // TEST4_H
