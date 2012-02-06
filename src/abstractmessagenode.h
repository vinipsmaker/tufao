#ifndef TUFAO_ABSTRACTMESSAGEDISPATCHER_H
#define TUFAO_ABSTRACTMESSAGEDISPATCHER_H

#include <QtCore/QObject>

namespace Tufao {

class AbstractMessageNode : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMessageNode(QObject *parent = 0);

signals:
    void newMessage(QByteArray msg);

public slots:
    virtual bool sendMessage(const QByteArray &msg) = 0;
};

} // namespace Tufao

#endif // TUFAO_ABSTRACTMESSAGEDISPATCHER_H
