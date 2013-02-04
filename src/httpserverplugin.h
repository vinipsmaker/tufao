#ifndef TUFAO_HTTPSERVERPLUGIN_H
#define TUFAO_HTTPSERVERPLUGIN_H

#include <QtCore/QVariant>
#include <QtCore/QObject>
#include "tufao_global.h"
#include <functional>

namespace Tufao {

class HttpServerRequest;
class HttpServerResponse;

/*!
  This class provides a factory interface to create request handlers and
  comunicate with factories of other plugins.

  An example follows:

  \include plugin.h

  And its implementation file:

  \include plugin.cpp

  \since
  1.0
  */
class TUFAO_EXPORT HttpServerPlugin
{
public:
    /*!
      Creates a persistent handler.

      \note
      The handler created will be used as argument in
      HttpServerRequestRouter::map. If you want to use a different handler to
      every request, you should create another handler in the body of the
      returned handler.
      */
    virtual std::function<bool(HttpServerRequest&, HttpServerResponse&)>
    createHandler(const QHash<QString, HttpServerPlugin*> &dependencies,
                  const QVariant &customData = QVariant()) = 0;
};

} // namespace Tufao

Q_DECLARE_INTERFACE(Tufao::HttpServerPlugin, "Tufao::HttpServerPlugin/1.0")

#endif // TUFAO_HTTPSERVERPLUGIN_H
