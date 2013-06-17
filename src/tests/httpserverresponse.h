#include <QtCore/QObject>

class HttpServerResponseTest: public QObject
{
    Q_OBJECT
private slots:
    void statusCode_data();
    void statusCode();
    void customReasonPhrases_data();
    void customReasonPhrases();
    void normalMessages_data();
    void normalMessages();
    void chunkedEntities_data();
    void chunkedEntities();
    void httpContinue();
    void options();
    void invalid();
};
