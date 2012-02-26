#ifndef TESTSGUIDE_H
#define TESTSGUIDE_H

#include <QObject>
#include "abstracttest.h"
#include <Tufao/HttpServer>
#include "test8.h"

class TestsGuide : public Tufao::HttpServer
{
    Q_OBJECT
public:
    explicit TestsGuide(QObject *parent = 0);

private slots:
    void onRequestReady(Tufao::HttpServerRequest *request,
                        Tufao::HttpServerResponse *response);
    void onTestReady();

protected:
    void upgrade(Tufao::HttpServerRequest *request, const QByteArray &head);

private:
    QList<AbstractTest *> tests;
    int completed;
    Test8 *upgradeTest;
};

#endif // TESTSGUIDE_H
