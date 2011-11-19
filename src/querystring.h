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

#ifndef TUFAO_QUERYSTRING_H
#define TUFAO_QUERYSTRING_H

#include <QMap>

class QByteArray;

namespace Tufao {

/*!
  This namespace provides utilities for dealing with query strings.
  */
namespace QueryString {

/*!
  Serialize an object to a query string.

  \param obj The object to be serialized.
  \param sep The separator character.
  \param eq The assignment character.
  \param percentEncoding true if the function should escape the characters.
  \param percent It allows you to replace the percent sign character for
  another.
  \return The serialized string.
  */
QByteArray stringify(QMap<QByteArray, QByteArray> obj, char sep = '&',
                     char eq = '=', bool percentEncoding = true,
                     char percent = '%');

/*!
  Deserialize a query string to an object.

  \param string The string to be parsed.
  \param sep The separator character.
  \param eq The assignment character.
  \param percentEncoding true if the function should escape the characters.
  \param percent It allows you to replace the percent sign character for
  another.
  \return The deserialized object.
  */
QMap<QByteArray, QByteArray> parse(const QByteArray &string, char sep = '&',
                                   char eq = '=', bool percentEncoding = true,
                                   char percent = '%');

} // namespace QueryString

} // namespace Tufao

#endif // TUFAO_QUERYSTRING_H
