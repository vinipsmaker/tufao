#ifndef ABSTRACTTEST_H
#define ABSTRACTTEST_H

#include <QObject>

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

} // namespace Tufao

class AbstractTest : public QObject
{
    Q_OBJECT
public:
    explicit AbstractTest(QObject *parent = 0);

    virtual void operator ()() = 0;

    virtual void testRequest(Tufao::HttpServerRequest *request,
                             Tufao::HttpServerResponse *response) = 0;

signals:
    void ready();
};

#endif // ABSTRACTTEST_H
