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

#ifndef TUFAO_QUERYSTRING_H
#define TUFAO_QUERYSTRING_H

#include "tufao_global.h"
#include <QtCore/QMap>

class QByteArray;

namespace Tufao {

/*!
  This namespace provides utilities for dealing with query strings.
  */
namespace QueryString {

/*!
  Serialize an object to a query string.

  \param map The object to be serialized.
  \param sep The separator character.
  \param eq The assignment character.
  \param percentEncoding true if the function should escape the characters.
  You should set it to false if you know there is no percent encoding applied
  and need some performance improvement.
  \param percent It allows you to replace the percent sign character for
  another.
  \return The serialized string.
  */
TUFAO_EXPORT QByteArray stringify(const QMap<QByteArray, QByteArray> &map,
                                  char sep = '&', char eq = '=',
                                  bool percentEncoding = true,
                                  char percent = '%');

/*!
  Deserialize a query string to an object.

  \param string The string to parse.
  \param sep The separator character.
  \param eq The assignment character.
  \param percentEncoding true if the function should escape the characters.
  You should set it to false if you know there is no percent encoding applied
  and need some performance improvement.
  \param percent It allows you to replace the percent sign character for
  another.
  \return The deserialized object.
  */
TUFAO_EXPORT QMap<QByteArray, QByteArray> parse(const QByteArray &string,
                                                char sep = '&', char eq = '=',
                                                bool percentEncoding = true,
                                                char percent = '%');

} // namespace QueryString

} // namespace Tufao

#endif // TUFAO_QUERYSTRING_H
