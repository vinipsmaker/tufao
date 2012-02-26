#include <QCoreApplication>
#include <Tufao/HttpServer>
#include "testsguide.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestsGuide t;
    Q_UNUSED(t)

    return a.exec();
}
