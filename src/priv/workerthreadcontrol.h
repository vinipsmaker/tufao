#ifndef TUFAO_PRIV_WORKERTHREADCONTROL_H
#define TUFAO_PRIV_WORKERTHREADCONTROL_H

#include <QObject>

namespace Tufao {

class WorkerThreadControl : public QObject
{
        Q_OBJECT
    public:
        explicit WorkerThreadControl(QObject *parent = 0);
        
    signals:
        
    public slots:
        void onResponseFinished();
        void onRequestDestroyed();
        void onRequestClosed();
        
};

} //namespace Tufao

#endif // TUFAO_PRIV_WORKERTHREADCONTROL_H
