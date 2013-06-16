#include <QtCore/QObject>

class CryptographyTest: public QObject
{
    Q_OBJECT
private slots:
    void hmacSha1_data();
    void hmacSha1();
};
