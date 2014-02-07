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

#ifndef TUFAO_HEADERS_H
#define TUFAO_HEADERS_H

#include <QtCore/QMultiHash>
#include <QtCore/QDateTime>
#include "ibytearray.h"

class QDateTime;

namespace Tufao {

/*!
  This class provides a representation of HTTP headers.

  HTTP headers are string-based properties with case-insensitive keys.

  \sa
  Tufao::IByteArray
  */
struct Headers: public QMultiHash<IByteArray, QByteArray>
{
    /*!
      Returns a RFC 1123 date time formatted string if \p dateTime.

      It's the standard date time format in HTTP.

      \since 0.3
      */
    TUFAO_EXPORT static QByteArray fromDateTime(const QDateTime &dateTime);

    /*!
      Try to decode \p headerValue using the most common http date time formats.

      These formats are:
        - RFC 1123
        - RFC 1036
        - ANSI C's asctime()

      \returns the converted QDateTime object or \p defaultValue if the
      conversion fails.

      \since 0.3
      */
    TUFAO_EXPORT static QDateTime toDateTime(const QByteArray &headerValue,
                                             const QDateTime &defaultValue
                                             = QDateTime());
};

TUFAO_EXPORT QDebug operator<<(QDebug dbg, const Headers &headers);

} // namespace Tufao

#endif // TUFAO_HEADERS_H
