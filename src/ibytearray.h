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
  This class provides a case insensitive QByteArray.

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
    operator QByteArray() const;

    bool operator !=(const IByteArray &ba) const;
    bool operator <(const IByteArray &ba) const;
    bool operator <=(const IByteArray &ba) const;
    IByteArray &operator =(const IByteArray &ba);
    bool operator == (const IByteArray &ba) const;
    bool operator >(const IByteArray &ba) const;
    bool operator >=(const IByteArray &ba) const;

    bool operator !=(const QByteArray &ba) const;
    bool operator <(const QByteArray &ba) const;
    bool operator <=(const QByteArray &ba) const;
    IByteArray &operator =(const QByteArray &ba);
    bool operator == (const QByteArray &ba) const;
    bool operator >(const QByteArray &ba) const;
    bool operator >=(const QByteArray &ba) const;
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

inline IByteArray::operator QByteArray() const
{
    return QByteArray(*this);
}

inline bool IByteArray::operator !=(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) != 0;
}

inline bool IByteArray::operator <(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) < 0;
}

inline bool IByteArray::operator <=(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) <= 0;
}

inline IByteArray &IByteArray::operator =(const IByteArray &ba)
{
    static_cast<QByteArray&>(*this) = ba;
    return *this;
}

inline bool IByteArray::operator ==(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) == 0;
}

inline bool IByteArray::operator >(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) > 0;
}

inline bool IByteArray::operator >=(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) >= 0;
}

inline bool IByteArray::operator !=(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) != 0;
}

inline bool IByteArray::operator <(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) < 0;
}

inline bool IByteArray::operator <=(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) <= 0;
}

inline IByteArray &IByteArray::operator =(const QByteArray &ba)
{
    static_cast<QByteArray&>(*this) = ba;
    return *this;
}

inline bool IByteArray::operator ==(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) == 0;
}

inline bool IByteArray::operator >(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) > 0;
}

inline bool IByteArray::operator >=(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) >= 0;
}

} // namespace Tufao

#endif // TUFAO_IBYTEARRAY_H
