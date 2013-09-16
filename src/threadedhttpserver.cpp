#include "threadedhttpserver.h"

namespace Tufao {

ThreadedHttpServer::ThreadedHttpServer()
{
}

void ThreadedHttpServer::incomingConnection(qintptr socketDescriptor)
{
}

void ThreadedHttpServer::checkContinue(HttpServerRequest &, HttpServerResponse &)
{
    return;
}

} // namespace Tufao
