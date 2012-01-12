#ifndef TEST7_H
#define TEST7_H

#include "abstracttest.h"
#include "stringlistsocket.h"
#include <QTimer>

class Test7 : public AbstractTest
{
    Q_OBJECT
public:
    explicit Test7(QObject *parent = 0);

    void operator ()();

    void testRequest(Tufao::HttpServerRequest *request,
                     Tufao::HttpServerResponse *response);

private slots:
    void onDisconnected();
    void onTimer();

private:
    StringListSocket *socket;
    QTimer timer;
    bool timeout;
};

#endif // TEST7_H
