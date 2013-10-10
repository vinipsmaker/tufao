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

#include <functional>
#include <initializer_list>

#include <QtCore/QObject>
#include <QtCore/QRegularExpression>

#include "abstracthttpupgradehandler.h"

namespace Tufao {

/*!
  This class provides a robust and high performance HTTP request router. It
  allows to register a chain of upgrade handlers. This router uses mapping rules
  based on the url's path component to determine the correct handlers.

  The type of mapping rules used in this class provides a predictable behaviour
  that is simple to understand and allow the use of caching algorithms to
  improve the performance.

  When the router finds one matching request handler, it will call it. If the
  found handler cannot handle the request (this is indicated by the return
  value), the router will continue its quest in the search of a worthy handler.
  If the router fails in its quest (when no handlers are found or when none of
  the found handlers are able to respond the request), the handleRequest method
  in the router returns false and the connection remains open.

  The code below provides an example usage:

  \include httpupgraderouter.cpp

  \sa
  HttpServer::setUpgradeHandler

  \since
  1.0
  */
class TUFAO_EXPORT HttpUpgradeRouter: public QObject,
                                      public AbstractHttpUpgradeHandler
{
    Q_OBJECT
public:
    /*!
     It's a simple typedef for the type of handler accepted by the
     HttpUpgradeRouter.

     \since
     1.0
     */
    typedef std::function<bool(HttpServerRequest&, const QByteArray &head)>
        Handler;

    /*!
      This class describes a request handler and a filter.

      The filter is very basic and only can select requests based on the url's
      path component.

      The handlers return a boolean indicating whether it's able to handle the
      request. You can use this return value to overcome the simplistic nature
      of the provided filter.
     */
    struct Mapping
    {
        /*!
          Constructs a Mapping object using \p path as filter and \p handler as
          handler.
         */
        Mapping(QRegularExpression path, Handler handler) :
            path(path), handler(handler)
        {}

        /*!
          Constructs an empty Mapping object.
         */
        Mapping() = default;

        /*!
          This attribute is used to filter requests based on the url's path
          component. It's a regular expression, so you have powerful control.
         */
        QRegularExpression path;
        Handler handler;
    };

    /*!
      Constructs a HttpServerRequestRouter object.
      */
    explicit HttpUpgradeRouter(QObject *parent = 0);

    /*!
      Constructs a HttpServerRequestRouter object initialized with \p mappings.
      */
    explicit HttpUpgradeRouter(std::initializer_list<Mapping> mappings,
                               QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpUpgradeRouter();

    /*!
      Chain \p map to the list of handlers available to handle requests.

      \return
      The index of the new mapping.
     */
    int map(Mapping map);

    /*!
      Chain \p map to the list of handlers available to handle requests.

      \return
      The index of the first element in the new mapping range.
     */
    int map(std::initializer_list<Mapping> map);

    /*!
      Removes the mapping at \p index.
     */
    void unmap(int index);

    /*!
      Removes all mappings.
      */
    void clear();

public slots:
    /*!
      It will route the request to the right handler.

      The handler will have access to the list of captured texts by the regular
      expression using HttpServerRequest::customData.

      \note
      The router won't touch the HttpServerRequest::customData if the regex
      don't capture any text.

      See example below:

      \include custom_data.cpp

      If there is already an object set for this request, the router will do the
      following steps:

      1. If the object is not a QVariantMap, override it
      2. If the object already has a item with the key "args", but the value is
         not a QStringList, override the item
      3. Append the list of captured texts in the object["args"]

      \note
      The request's custom data is copied at the beginning and is used to
      restore the custom data state before call every handler. The state will
      also be restored if no handlers are capable of handle the request.

      \return
      Returns true if one handler able to respond the request is found.

      \since
      1.0
      */
    bool handleUpgrade(Tufao::HttpServerRequest &request,
                       const QByteArray &head) override;

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPUPGRADEROUTER_H
