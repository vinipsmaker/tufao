#include <QtCore/QObject>

class UrlTest: public QObject
{
    Q_OBJECT
private slots:
    void parse_data();
    void parse();
};
