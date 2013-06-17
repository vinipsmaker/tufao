#include <QtCore/QObject>

class AsctimeTest: public QObject
{
    Q_OBJECT
private slots:
    void dateTimeConversion_data();
    void dateTimeConversion();
};
