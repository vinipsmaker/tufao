/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

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

#ifndef TUFAO_ABSTRACTHTTPSERVERREQUESTHANDLERFACTORY_H
#define TUFAO_ABSTRACTHTTPSERVERREQUESTHANDLERFACTORY_H

#include <QtCore/QObject>
#include "tufao_global.h"

namespace Tufao {

class AbstractHttpServerRequestHandler;

/*!
  This class provides an factory interface to create
  AbstractHttpServerRequestHandler objects.

  If you want to implement plugins to interact with HttpPluginServer, then you
  need to make your plugins implement this interface.

  \include plugin.h

  \include plugin.cpp

  \since
  0.3
  */
class TUFAO_EXPORT AbstractHttpServerRequestHandlerFactory
{
public:
    /*!
      Creates one handler.

      All handlers created by this method should return objects with similar
      behaviour (e.g. returning objects from the same class).
      */
    virtual AbstractHttpServerRequestHandler
    *createHandler(QObject *parent) = 0;
};

} // namespace Tufao

Q_DECLARE_INTERFACE(Tufao::AbstractHttpServerRequestHandlerFactory,
                    "Tufao::AbstractHttpServerRequestHandlerFactory/0.1")

#endif // TUFAO_ABSTRACTHTTPSERVERREQUESTHANDLERFACTORY_H
