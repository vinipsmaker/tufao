/*
  Copyright (c) 2013 Benjamin Zeller

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */
#ifndef TUFAO_THREADEDHTTPSERVER_H
#define TUFAO_THREADEDHTTPSERVER_H

#include "httpserver.h"
#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

class AbstractConnectionHandler;

class ThreadedHttpServer : public QObject
{
    public:
        struct Priv;
        /*!
         It's a simple typedef for the type of handler factory accepted by the
         ThreadedHttpServer.
         */
        typedef std::function<AbstractConnectionHandler* ( )>
        ConnectionHandlerFactory;

        /*!
         It's a simple typedef for the type of factory accepted by the
         ThreadedHttpServer.
         */
        typedef std::function<void (AbstractConnectionHandler* connHandler, void **customData)>
        RequestHandlerFactory;

        /*!
         It's a simple typedef for the type of cleanup function accepted by the
         ThreadedHttpRequestDispatcher.
         */
        typedef std::function<void (void **customData)>
        CleanupHandlerFactory;

        /*!
          Constructs a Tufao::ThreadedHttpServer object.

          \p parent is passed to the QObject constructor.
          */
        explicit ThreadedHttpServer(QObject* parent = 0);

        /*!
          Destroys the object.
          */
        virtual ~ThreadedHttpServer();

        /*!
          Tells the server to listen for incoming connections on address \p address
          and port \p port.

          If \p port is 0, a port is chosen automatically. The default registered
          port to HTTP server is 80.

          If \p address is QHostAddress::Any, the server will listen on all network
          interfaces.

          \return true on success

          \sa
          Tufao::HttpServer::isListening
          Tufao::HttpServer::serverPort
          */
        bool listen(const QHostAddress &address = QHostAddress::Any,
                    quint16 port = 0);

        /*!
          Returns true if the server is listening for incoming connections.
          */
        bool isListening() const;

        /*!
          Returns the server's port if the server is listening; otherwise returns 0.

          \sa
          Tufao::HttpServer::listen
          Tufao::HttpServer::isListening
          */
        quint16 serverPort() const;


        /*!
          \brief setConnectionHandlerFactory
          Sets the factory that is used to create a ConnectionHandler per thread
          \note changing this after entering the eventloop requires to call restart()
         */
        void setConnectionHandlerFactory (ConnectionHandlerFactory factory);

        /*!
          \brief setRequestHandlerFactory
          Sets the factory that is used to create a RequestHandler per thread
          \note changing this after entering the eventloop requires to call restart()
         */
        void setRequestHandlerFactory    (RequestHandlerFactory factory);

        /*!
          \brief setCleanupHandlerFactory
          Sets the cleanup function that is called when the thread is shutdown
          \note changing this after entering the eventloop requires to call restart()
         */
        void setCleanupHandlerFactory    (CleanupHandlerFactory factory);

        /*!
          \brief setThreadPoolSize
          Sets the number of threads that handle incoming requests
          \param size
          \note changing this after start was called requires to call restart()
         */
        void setThreadPoolSize (const unsigned int size);

        /*!
          \brief restart
          Will unload all threads and restart them

          \note This will block until all threads are restarted
         */
        void restart ();

        static ConnectionHandlerFactory defaultConnectionHandlerFactory();

        void shutdown();
    public slots:
        /*!
          Closes the server. The server will no longer listen for incoming
          connections.
          */
        void close();

    protected slots:
        void onNewConnection(qintptr socketDescriptor);

    protected:
        explicit ThreadedHttpServer(Priv* priv, QObject* parent = 0);
        Priv *_priv();
        const Priv *_priv() const;

    private:
        Priv *priv;
};

/**
 * @brief tDebug
 * @return
 * Calls qDebug() but prefixes the string with the threadId
 */
TUFAO_EXPORT QDebug tDebug ();

} // namespace Tufao

#endif // TUFAO_THREADEDHTTPSERVER_H
