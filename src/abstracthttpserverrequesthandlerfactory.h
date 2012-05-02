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
