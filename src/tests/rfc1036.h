#include <QtCore/QObject>

class Rfc1036Test: public QObject
{
    Q_OBJECT
private slots:
    void dateTimeConversion_data();
    void dateTimeConversion();
};
