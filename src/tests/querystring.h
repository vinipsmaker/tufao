#include <QtCore/QObject>

class QueryStringTest: public QObject
{
    Q_OBJECT
private slots:
    void parse_data();
    void parse();

    void stringify_data();
    void stringify();
};
