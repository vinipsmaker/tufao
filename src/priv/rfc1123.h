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

#ifndef TUFAO_PRIV_RFC1123_H
#define TUFAO_PRIV_RFC1123_H

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

namespace Tufao {

class Q_DECL_EXPORT Rfc1123
{
public:
    Rfc1123(const QString &headerValue);

    explicit operator bool();

    QDateTime operator ()();

private:
    static const QRegularExpression rfc1123;
    static const QStringList months;

    QDateTime dateTime;
    bool valid;
};

inline Rfc1123::Rfc1123(const QString &headerValue) :
    valid(true)
{
    QRegularExpressionMatch match{rfc1123.match(headerValue)};
    if (!match.hasMatch()) {
        valid = false;
        return;
    }

    int year;
    {
        QString yearStr = match.captured(3);
        if (yearStr.size() == 2)
            year = yearStr.toInt() + 1900;
        else
            year = yearStr.toInt();
    }

    int month = months.indexOf(match.captured(2));
    if (month == -1) {
        valid = false;
        return;
    } else {
        ++month;
    }

    int day = match.captured(1).toInt();
    int hours = match.captured(4).toInt();
    int minutes = match.captured(5).toInt();
    int seconds = 0;
    {
        QString secondsStr(match.captured(6));
        if (!secondsStr.isEmpty())
            seconds = secondsStr.toInt();
    }

    dateTime = QDateTime(QDate(year, month, day),
                         QTime(hours, minutes, seconds),
                         Qt::UTC);
}

inline Rfc1123::operator bool()
{
    return valid;
}

inline QDateTime Rfc1123::operator ()()
{
    return dateTime;
}

} // namespace Tufao

#endif // TUFAO_PRIV_RFC1123_H
