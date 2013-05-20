#include <QtCore/QObject>

class IByteArrayTest: public QObject
{
    Q_OBJECT
private slots:
    void relational_data();
    void relational();
    void caseInsensitivity_data();
    void caseInsensitivity();
};
