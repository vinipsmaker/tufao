#ifndef TUFAO_ABSTRACTMESSAGEDISPATCHER_H
#define TUFAO_ABSTRACTMESSAGEDISPATCHER_H

#include <QtCore/QObject>

namespace Tufao {

class AbstractMessageSocket : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMessageSocket(QObject *parent = 0);

signals:
    void connected();
    void disconnected();
    void newMessage(QByteArray msg);

public slots:
    virtual void close() = 0;
    virtual bool sendMessage(const QByteArray &msg) = 0;
};

} // namespace Tufao

#endif // TUFAO_ABSTRACTMESSAGEDISPATCHER_H
