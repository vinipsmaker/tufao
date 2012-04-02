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

#ifndef TUFAO_ABSTRACTHTTPSERVERREQUESTHANDLERLOADER_H
#define TUFAO_ABSTRACTHTTPSERVERREQUESTHANDLERLOADER_H

#include <QtCore/QObject>
#include "tufao_global.h"

namespace Tufao {

class AbstractHttpServerRequestHandler;

class TUFAO_EXPORT AbstractHttpServerRequestHandlerLoader
{
public:
    virtual AbstractHttpServerRequestHandler
    *createHandler(QObject *parent) = 0;
};

} // namespace Tufao

Q_DECLARE_INTERFACE(Tufao::AbstractHttpServerRequestHandlerLoader,
                    "Tufao::AbstractHttpServerRequestHandlerLoader/0.1")

#endif // TUFAO_ABSTRACTHTTPSERVERREQUESTHANDLERLOADER_H
