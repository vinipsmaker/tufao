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

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

namespace Tufao {

class Q_DECL_EXPORT Rfc1036
{
public:
    Rfc1036(const QString &headerValue);

    explicit operator bool();

    QDateTime operator ()();

private:
    static const QRegularExpression rfc1036;
    static const QStringList months;

    QDateTime dateTime;
    bool valid;
};

inline Rfc1036::Rfc1036(const QString &headerValue) :
    valid(true)
{
    QRegularExpressionMatch match{rfc1036.match(headerValue)};
    if (!match.hasMatch()) {
        valid = false;
        return;
    }

    int month = months.indexOf(match.captured(2));
    if (month == -1) {
        valid = false;
        return;
    } else {
        ++month;
    }

    int day = match.captured(1).toInt();
    int year = match.captured(3).toInt() + 1900;
    int hours = match.captured(4).toInt();
    int minutes = match.captured(5).toInt();
    int seconds = match.captured(6).toInt();

    dateTime = QDateTime(QDate(year, month, day),
                         QTime(hours, minutes, seconds),
                         Qt::UTC);
}

inline Rfc1036::operator bool()
{
    return valid;
}

inline QDateTime Rfc1036::operator ()()
{
    return dateTime;
}

} // namespace Tufao

#endif // TUFAO_PRIV_RFC1036_H
