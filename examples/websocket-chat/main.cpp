#include <QCoreApplication>
#include "webserver.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QCoreApplication a(argc, argv);
    WebServer server;

    return a.exec();
}
