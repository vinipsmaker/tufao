#ifndef STRINGLISTSOCKET_H
#define STRINGLISTSOCKET_H

#include <QTcpSocket>
#include <QTimer>

class StringListSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit StringListSocket(const QList<QByteArray> &data,
                              QObject *parent = 0);

signals:
    void testReady();

protected slots:
    void onConnected();
    void onTimer();

private:
    QList<QByteArray> data;
    int index;
    QTimer timer;
};

#endif // STRINGLISTSOCKET_H
