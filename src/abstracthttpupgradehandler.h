/*
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

#ifndef TUFAO_ABSTRACTHTTPUPGRADEHANDLER_H
#define TUFAO_ABSTRACTHTTPUPGRADEHANDLER_H

#include <functional>
#include <QtNetwork/QAbstractSocket>
#include "httpserverrequest.h"

namespace Tufao {

/*!
  This class provides an interface for HTTP Upgrade handlers. An upgrade
  handler is usually registered to handle requests matching some set of rules
  and usually used with a set of other handlers.

  A sample upgrade handler is given below:

  \include upgradehandler.cpp

  \sa
  HttpUpgradeRouter

  \since
  0.6
  */
class TUFAO_EXPORT AbstractHttpUpgradeHandler
{
public:
    ~AbstractHttpUpgradeHandler() = default;

    /*!
      Implicit conversion operator to std::function functor object.

      \warning
      You shall not use the returned object after the AbstractHttpUpgradeHandler
      object is destroyed.

      \since
      1.0
     */
    operator std::function<bool(HttpServerRequest&, const QByteArray&)>();

    /*!
      Implicit conversion operator to std::function functor object.

      \note
      The returned functor object calls handleUpgrade. If handleUpgrade returns
      false, then it will close the connection.

      \warning
      You shall not use the returned object after the AbstractHttpUpgradeHandler
      object is destroyed.

      \since
      1.0
     */
    operator std::function<void(HttpServerRequest&, const QByteArray&)>();

    /*!
      Handles the HTTP \p request.

      \retval true
      If the handler has upgraded the connection to the request protocol.

      \retval false
      If the handler didn't change the protocol to the requested one. The
      HTTP session should remain open (eg. a response message shouldn't be
      sent), leaving the response free to be used by other handlers in the
      chain.

      \since
      1.0
      */
    virtual bool handleUpgrade(Tufao::HttpServerRequest &request,
                               const QByteArray &head) = 0;
};

inline AbstractHttpUpgradeHandler::operator
std::function<bool(HttpServerRequest&, const QByteArray&)>()
{
    return [this](HttpServerRequest &req, const QByteArray &head) {
        return this->handleUpgrade(req, head);
    };
}

inline AbstractHttpUpgradeHandler::operator
std::function<void(HttpServerRequest&, const QByteArray&)>()
{
    return [this](HttpServerRequest &req, const QByteArray &head) {
        if (!this->handleUpgrade(req, head))
            req.socket().close();
    };
}

} // namespace Tufao

#endif // TUFAO_ABSTRACTHTTPUPGRADEHANDLER_H
