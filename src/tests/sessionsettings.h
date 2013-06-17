#include <QtCore/QObject>

class SessionSettingsTest: public QObject
{
    Q_OBJECT
private slots:
    void cookie_data();
    void cookie();

private:
    static void fillCookieData(int timeout, bool httpOnly, bool secure);
};
