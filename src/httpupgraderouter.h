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

#ifndef TUFAO_HTTPUPGRADEROUTER_H
#define TUFAO_HTTPUPGRADEROUTER_H

#include "abstracthttpupgradehandler.h"

namespace Tufao {

/*!
  This class provides a robust and high performance HTTP upgrade router. It
  allows register AbstractHttpUpgradeHandler objects to handle requests
  that matches the mapping rules. The mapping rules are limited to use regular
  expressions to filter the url path component.

  The type of mapping rules used in this class provides a predictable behaviour
  that is simple to understand and allow the use of caching algorithms to
  improve the performance.

  Did you note that HttpServerRequestRouter is a subclass of
  AbstractHttpUpgradeHandler? This design choice allows you to nest
  routes.

  When the router finds one matching request handler, it will call its
  handleRequest method passing the request and head objects and also the
  captured texts by the regular expression. If the found handler cannot handle
  the request (this is indicated by the return value), the router will continue
  its quest in the search of a worthy handler. If the router fails in its quest
  (when no handlers are found or when none of the found handlers are able to
  respond the request), the handleRequest method in the router returns false and
  the connection remains open. This mean that you should always create a handler
  that closes any request as the last handler in the most top-level request
  router.

  \since
  0.6
  */
class TUFAO_EXPORT HttpUpgradeRouter: public AbstractHttpUpgradeHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a HttpUpgradeRouter object.
      */
    explicit HttpUpgradeRouter(QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpUpgradeRouter();

    /*!
      This method maps requests which path components in the url matches the
      regular expression \p path to the given \p handler.

      \return
      Returns itself, so you can create several mappings in the same line of
      code.
      */
    HttpUpgradeRouter &map(const QRegExp &path,
                           AbstractHttpUpgradeHandler *handler);

    /*!
      Removes all mappings that correspond to \p path and \p handler rules and
      aren't specific to a particular HTTP method.
      */
    HttpUpgradeRouter &unmap(const QRegExp &path,
                             AbstractHttpUpgradeHandler *handler);

    /*!
      Removes all mappings that correspond to \p path rule.
      */
    HttpUpgradeRouter &unmap(const QRegExp &path);

    /*!
      Removes all mappings that correspond to \p handler rule.
      */
    HttpUpgradeRouter &unmap(AbstractHttpUpgradeHandler *handler);

    /*!
      Removes all mappings.
      */
    void clear();

public slots:
    /*!
      It will route the request to the right handler. To route the request,
      it'll percent decode the path component from the url.

      The \p args object is prepend in the list of captured texts by the regular
      expression and then passed to the handler. My advice is to don't overuse
      this feature or you will get a code difficult to understand.

      \return
      Returns true if one handler able to respond the request is found.
      */
    bool handleRequest(Tufao::HttpServerRequest *request,
                       const QByteArray &head,
                       const QStringList &args = QStringList());

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPUPGRADEROUTER_H
