#ifndef ABSTRACTCONNECTIONHANDLER_P_H
#define ABSTRACTCONNECTIONHANDLER_P_H

#include "../abstractconnectionhandler.h"

namespace Tufao {

struct AbstractConnectionHandler::Priv
{
    Priv();
    virtual ~Priv();
    int timeout;
};

}

#endif // ABSTRACTCONNECTIONHANDLER_P_H
