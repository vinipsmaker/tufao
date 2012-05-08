/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_PRIV_RFC1036_H
#define TUFAO_PRIV_RFC1036_H

#include <QtCore/QRegExp>
#include <QtCore/QDateTime>

namespace Tufao {

class Rfc1036
{
public:
    Rfc1036(const QByteArray &headerValue);

    operator bool();

    QDateTime operator ()();

private:
    static const QRegExp rfc1036;

    const QByteArray &headerValue;
    QRegExp regexp;
};

inline Rfc1036::Rfc1036(const QByteArray &headerValue) :
    headerValue(headerValue),
    regexp(rfc1036)
{}

inline Rfc1036::operator bool()
{
    return regexp.indexIn(headerValue) != -1;
}

inline QDateTime Rfc1036::operator ()()
{
    if (!regexp.captureCount())
        return QDateTime();

    int year = regexp.cap(3).toInt() + 1900, month = 1,
            day = regexp.cap(1).toInt();
    {
        QString monthStr(regexp.cap(2));
        if (monthStr == "Jan")
            month = 1;
        else if (monthStr == "Feb")
            month = 2;
        else if (monthStr == "Mar")
            month = 3;
        else if (monthStr == "Apr")
            month = 4;
        else if (monthStr == "May")
            month = 5;
        else if (monthStr == "Jun")
            month = 6;
        else if (monthStr == "Jul")
            month = 7;
        else if (monthStr == "Aug")
            month = 8;
        else if (monthStr == "Sep")
            month = 9;
        else if (monthStr == "Oct")
            month = 10;
        else if (monthStr == "Nov")
            month = 11;
        else if (monthStr == "Dec")
            month = 12;
    }

    int hours = regexp.cap(4).toInt(), minutes = regexp.cap(5).toInt(),
            seconds = regexp.cap(6).toInt();

    return QDateTime(QDate(year, month, day), QTime(hours, minutes, seconds),
                     Qt::UTC);
}

} // namespace Tufao

#endif // TUFAO_PRIV_RFC1036_H
