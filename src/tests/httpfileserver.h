#include <QtCore/QObject>

class HttpFileServerTest: public QObject
{
    Q_OBJECT
private slots:
    void properties_data();
    void properties();
};
