#include <QtCore/QObject>

class HeadersTest: public QObject
{
    Q_OBJECT
private slots:
    void caseInsensitivity_data();
    void caseInsensitivity();
    void dateTimeConversion_data();
    void dateTimeConversion();
};
