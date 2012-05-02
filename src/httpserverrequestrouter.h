/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_HTTPSERVERREQUESTROUTER_H
#define TUFAO_HTTPSERVERREQUESTROUTER_H

#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

/*!
  This class provides a robust and high performance HTTP request router. It
  allows register AbstractHttpServerRequestHandler objects to handle requests
  that matches the mapping rules. The mapping rules are limited to use regular
  expressions to filter the url path component and, optionally, specify which
  method the object can handle.

  The type of mapping rules used in this class provides a predictable behaviour
  that is simple to understand and allow the use of caching algorithms to
  improve the performance.

  Did you note that HttpServerRequestRouter is a subclass of
  AbstractHttpServerRequestHandler? This design choice allows you to nest
  routes. But the nicest thing (for you) may not be the nested routing
  possibility, but the interface that this class is implementing. This design
  allows you to use the same request routing object to handle the requests
  coming from a HttpServer and a HttpsServer objects (or how many of them you
  need).

  When the router finds one matching request handler, it will call its
  handleRequest method passing the request and response objects and also the
  captured texts by the regular expression. If the found handler cannot handle
  the request (this is indicated by the return value), the router will continue
  its quest in the search of a worthy handler. If the router fails in its quest
  (when no handlers are found or when none of the found handlers are able to
  respond the request), the handleRequest method in the router returns false and
  the connection remains open. This mean that you should always create a handler
  that responds to any request with a <i>404 not found</i> as the last handler
  in the most top-level request router.

  \since
  0.3
  */
class TUFAO_EXPORT HttpServerRequestRouter:
        public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a HttpServerRequestRouter object.
      */
    explicit HttpServerRequestRouter(QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpServerRequestRouter();

    /*!
      This method maps requests which path components in the url matches the
      regular expression \p path to the given \p handler.

      \return
      Returns itself, so you can create several mappings in the same line of
      code.
      */
    HttpServerRequestRouter &map(const QRegExp &path,
                                 AbstractHttpServerRequestHandler *handler);

    /*!
      This method maps requests which path components in the url matches the
      regular expression \p path and uses the HTTP verb \p method to the given
      \p handler.

      \note
      The router gives priority to maps that specified a method over the ones
      that didn't.

      \return
      Returns itself, so you can create several mappings in the same line of
      code.
      */
    HttpServerRequestRouter &map(const QRegExp &path, const QByteArray &method,
                                 AbstractHttpServerRequestHandler *handler);

    /*!
      Removes all mappings that correspond to \p path and \p handler rules and
      aren't specific to a particular HTTP method.
      */
    HttpServerRequestRouter &unmap(const QRegExp &path,
                                   AbstractHttpServerRequestHandler *handler);

    /*!
      Removes all mappings that correspond to \p path, \p method and \p handler
      rules.
      */
    HttpServerRequestRouter &unmap(const QRegExp &path,
                                   const QByteArray &method,
                                   AbstractHttpServerRequestHandler *handler);

    /*!
      Removes all mappings that correspond to \p path rule.
      */
    HttpServerRequestRouter &unmap(const QRegExp &path);

    /*!
      Removes all mappings that correspond to \p path and \p method rules.
      */
    HttpServerRequestRouter &unmap(const QRegExp &path,
                                   const QByteArray &method);

    /*!
      Removes all mappings that correspond to \p handler rule.
      */
    HttpServerRequestRouter &unmap(AbstractHttpServerRequestHandler *handler);

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
                       Tufao::HttpServerResponse *response,
                       const QStringList &args = QStringList());

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPSERVERREQUESTROUTER_H
