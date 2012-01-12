#ifndef TESTSGUIDE_H
#define TESTSGUIDE_H

#include <QObject>
#include "abstracttest.h"

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

} // namespace Tufao

class TestsGuide : public QObject
{
    Q_OBJECT
public:
    explicit TestsGuide(QObject *parent = 0);

private slots:
    void onRequestReady(Tufao::HttpServerRequest *request,
                        Tufao::HttpServerResponse *response);
    void onTestReady();

private:
    QList<AbstractTest *> tests;
    int completed;
};

#endif // TESTSGUIDE_H
