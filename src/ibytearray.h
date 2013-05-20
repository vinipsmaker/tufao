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

#ifndef TUFAO_IBYTEARRAY_H
#define TUFAO_IBYTEARRAY_H

#include <QtCore/QByteArray>
#include "tufao_global.h"

namespace Tufao {

/*!
  This class provides a case insensitive QByteArray. It inherits from QByteArray
  and provides non-member functions to overload the common operators:
    - operator !=
    - operator <
    - operator <=
    - operator ==
    - operator >
    - operator >=

  \note
  Use of overloaded operator '<' is intentionally ambiguous when you combine
  IByteArray and const char *. This design forces you to make your intent
  explicit using explicit casts.

  \par
  \note
  All member functions of this class are inlined and should add the minimum (if
  any) of overhead.
  */
class TUFAO_EXPORT IByteArray : public QByteArray
{
public:
    IByteArray();
    IByteArray(const QByteArray &ba);
    IByteArray(const char *str);
    IByteArray(const char *data, int size);
    IByteArray(int size, char ch);

    IByteArray &operator =(const QByteArray &ba);
};

inline IByteArray::IByteArray()
{}

inline IByteArray::IByteArray(const QByteArray &ba) :
    QByteArray(ba)
{}

inline IByteArray::IByteArray(const char *str) :
    QByteArray(str)
{}

inline IByteArray::IByteArray(const char *data, int size) :
    QByteArray(data, size)
{}

inline IByteArray::IByteArray(int size, char ch) :
    QByteArray(size, ch)
{}

inline IByteArray &IByteArray::operator =(const QByteArray &ba)
{
    static_cast<QByteArray&>(*this) = ba;
    return *this;
}

// Non-member functions:

inline bool operator !=(const IByteArray &lhs, const IByteArray &rhs)
{
    return qstricmp(lhs.constData(), rhs.constData()) != 0;
}

inline bool operator <(const IByteArray &lhs, const IByteArray &rhs)
{
    return qstricmp(lhs.constData(), rhs.constData()) < 0;
}

inline bool operator <=(const IByteArray &lhs, const IByteArray &rhs)
{
    return qstricmp(lhs.constData(), rhs.constData()) <= 0;
}

inline bool operator ==(const IByteArray &lhs, const IByteArray &rhs)
{
    return qstricmp(lhs.constData(), rhs.constData()) == 0;
}

inline bool operator >(const IByteArray &lhs, const IByteArray &rhs)
{
    return qstricmp(lhs.constData(), rhs.constData()) > 0;
}

inline bool operator >=(const IByteArray &lhs, const IByteArray &rhs)
{
    return qstricmp(lhs.constData(), rhs.constData()) >= 0;
}

inline uint qHash(const IByteArray &key)
{
    return qHash(key.toLower());
}

} // namespace Tufao

#endif // TUFAO_IBYTEARRAY_H
