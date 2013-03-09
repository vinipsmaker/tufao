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

/*!
  \file httpserverplugin.h
 */

#ifndef TUFAO_HTTPSERVERPLUGIN_H
#define TUFAO_HTTPSERVERPLUGIN_H

/*!
  This definition stores the Tufao::HttpServerPlugin interface IID.

  When you define a plugin, you'll use this macro as the IID argument of
  Q_PLUGIN_METADATA macro to tell the class is implementing the
  Tufao::HttpServerPlugin interface. Example:

  \include plugin.h
 */
#define TUFAO_HTTPSERVERPLUGIN_IID "Tufao::HttpServerPlugin/1.0"

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

  \sa
  TUFAO_HTTPSERVERPLUGIN_IID

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

Q_DECLARE_INTERFACE(Tufao::HttpServerPlugin, TUFAO_HTTPSERVERPLUGIN_IID)

#endif // TUFAO_HTTPSERVERPLUGIN_H
