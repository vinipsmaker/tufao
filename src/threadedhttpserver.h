#ifndef TUFAO_THREADEDHTTPSERVER_H
#define TUFAO_THREADEDHTTPSERVER_H

#include "httpserver.h"
#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

class ThreadedHttpServer : public HttpServer
{
    public:
        /*!
         It's a simple typedef for the type of factory accepted by the
         ThreadedHttpServer.
         */
        typedef std::function<AbstractHttpServerRequestHandler* (void **customData)>
        Factory;

        /*!
         It's a simple typedef for the type of cleanup function accepted by the
         ThreadedHttpRequestDispatcher.
         */
        typedef std::function<void (void **customData)>
        CleanupFunc;

        /*!
         It's a simple typedef for the type of factory accepted by the
         ThreadedHttpRequestDispatcher.
         */
        typedef std::function<HttpServer::UpgradeHandler ()>
        UpgradeHandlerFactory;

        ThreadedHttpServer();

        // HttpServer interface
    protected:
        virtual void incomingConnection(qintptr socketDescriptor);
        virtual void checkContinue(HttpServerRequest &, HttpServerResponse &);

    private:
        struct Priv;
        Priv *priv;
};

void ThreadedHttpServer::incomingConnection(qintptr socketDescriptor);

void ThreadedHttpServer::checkContinue(HttpServerRequest &, HttpServerResponse &);

} // namespace Tufao

#endif // TUFAO_THREADEDHTTPSERVER_H
