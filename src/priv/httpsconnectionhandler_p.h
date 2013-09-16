#ifndef HTTPSCONNECTIONHANDLER_P_H
#define HTTPSCONNECTIONHANDLER_P_H

#include "../httpsconnectionhandler.h"
#include "httpconnectionhandler_p.h"

#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

namespace Tufao{

struct HttpsConnectionHandler::Priv : public HttpConnectionHandler::Priv
{
    QSslCertificate localCertificate;
    QSslKey privateKey;
};

}
#endif // HTTPSCONNECTIONHANDLER_P_H
