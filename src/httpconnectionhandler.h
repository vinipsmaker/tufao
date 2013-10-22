/*
  Copyright (c) 2013 Benjamin Zeller
  Copyright (c) 2013 Vinícius dos Santos Oliveira

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

#ifndef TUFAO_HTTPCONNECTIONHANDLER_H
#define TUFAO_HTTPCONNECTIONHANDLER_H

#include <functional>
#include "abstractconnectionhandler.h"

namespace Tufao {

/*!
  The Tufao::HttpConnectionHandler class provides an implementation of the HTTP protocol.

  The HTTP is a stateless request-response based protoccol. It lets you create
  distributed dynamic collaborative applications.

  \sa
  Tufao::HttpServerRequest

  \sa
  Tufao::HttpServerResponse
  */

class TUFAO_EXPORT HttpConnectionHandler : public AbstractConnectionHandler
{
        Q_OBJECT

    public:
         struct Priv;
        /*!
          A typedef to http upgrade request handler.

          \sa
          AbstractHttpUpgradeHandler
          setUpgradeHandler

          \since
          1.0
         */
        typedef std::function<void(HttpServerRequest &request, const QByteArray&)>
            UpgradeHandler;

        explicit HttpConnectionHandler(QObject *parent = 0);

        /*!
          This method sets the handler that will be called to handle http upgrade
          requests.

          \note
          The connection object associated with request parameter
          (Tufao::HttpServerRequest::socket) will be deleted when disconnected. If
          you need to delete it sooner, just call QIODevice::close or
          QObject::deleteLater. \n \n

          \note
          If you pass an empty std::function object, this function does nothing.

          \sa
          defaultUpgradeHandler

          \since
          1.0
         */
        void setUpgradeHandler(UpgradeHandler functor);

        /*!
          Returns the default http upgrade request's handler.

          The default handler closes the connection.

          \since
          1.0
         */
        static UpgradeHandler defaultUpgradeHandler();
        
        // AbstractConnectionHandler interface
    public:
        virtual bool incomingConnection(qintptr socketDescriptor) override;
        virtual void closePendingConnection(qintptr socketDescriptor) override;

    protected:
        virtual void checkContinue(HttpServerRequest &request, HttpServerResponse &response) override;
        virtual void handleUpgrade(HttpServerRequest &request) override;
        HttpConnectionHandler(HttpConnectionHandler::Priv *p, QObject *parent);
};
} // namespace Tufao

#endif // TUFAO_HTTPCONNECTIONHANDLER_H
