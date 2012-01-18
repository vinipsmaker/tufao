/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#ifndef TUFAO_HEADERS_H
#define TUFAO_HEADERS_H

#include <QtCore/QMultiMap>
#include "ibytearray.h"

namespace Tufao {

/*!
  This class provides a representation of HTTP headers.

  HTTP headers are information stored in the form of an associative set, where
  the key and the value are strings, but the key is case insensitive.

  \sa
  Tufao::IByteArray
  */
struct Headers: public QMultiMap<IByteArray, QByteArray> {};

} // namespace Tufao

#endif // TUFAO_HEADERS_H
