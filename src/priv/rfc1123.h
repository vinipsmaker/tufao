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

#ifndef TUFAO_PRIV_RFC1123_H
#define TUFAO_PRIV_RFC1123_H

#include <QtCore/QRegExp>
#include <QtCore/QDateTime>

namespace Tufao {
namespace Priv {

class Rfc1123
{
public:
    Rfc1123(const QByteArray &headerValue);

    operator bool();

    QDateTime operator ()();

private:
    static const QRegExp rfc1123;

    const QByteArray &headerValue;
    QRegExp regexp;
};

inline Rfc1123::Rfc1123(const QByteArray &headerValue) :
    headerValue(headerValue),
    regexp(rfc1123)
{}

inline Rfc1123::operator bool()
{
    return regexp.indexIn(headerValue) != -1;
}

inline QDateTime Rfc1123::operator ()()
{
    if (!regexp.captureCount())
        return QDateTime();

    int year, month = regexp.cap(2).toInt(), day = regexp.cap(1).toInt();
    {
        QString yearStr(regexp.cap(3));
        if (yearStr.size() == 2)
            year = yearStr.toInt() + 1900;
        else
            year = yearStr.toInt();
    }

    int hours = regexp.cap(4).toInt(), minutes = regexp.cap(5).toInt(),
            seconds = 0;
    {
        QString secondsStr(regexp.cap(6));
        if (!secondsStr.isEmpty())
            seconds = secondsStr.toInt();
    }

    return QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds),
                     Qt::UTC);
}

} // namespace Priv
} // namespace Tufao

#endif // TUFAO_PRIV_RFC1123_H
