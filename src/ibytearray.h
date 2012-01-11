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

#ifndef TUFAO_IBYTEARRAY_H
#define TUFAO_IBYTEARRAY_H

#include <QByteArray>

namespace Tufao {

/*!
  This class provides a case insensitive QByteArray.

  \note
  All member functions of this class are inlined and should add the minimum (if
  any) of overhead.
  */
class IByteArray : public QByteArray
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
    return *this = ba;
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
    return *this = ba;
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
