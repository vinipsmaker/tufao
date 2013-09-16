#ifndef HTTPCONNECTIONHANDLER_P_H
#define HTTPCONNECTIONHANDLER_P_H

#include "../httpconnectionhandler.h"
#include "../httpserverrequest.h"
#include "abstractconnectionhandler_p.h"

#include <QtNetwork/QAbstractSocket>

namespace Tufao {

struct HttpConnectionHandler::Priv : public AbstractConnectionHandler::Priv
{
    Priv();

    UpgradeHandler upgradeHandler;
    static UpgradeHandler defaultUpgradeHandler;
};

}

#endif // HTTPCONNECTIONHANDLER_P_H
