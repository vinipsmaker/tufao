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

#ifndef TUFAO_PRIV_ASCTIME_H
#define TUFAO_PRIV_ASCTIME_H

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

namespace Tufao {

class Q_DECL_EXPORT Asctime
{
public:
    Asctime(const QString &headerValue);

    explicit operator bool();

    QDateTime operator ()();

private:
    static const QRegularExpression asctime;
    static const QStringList months;

    QDateTime dateTime;
    bool valid;
};

inline Asctime::Asctime(const QString &headerValue) :
    valid(true)
{
    QRegularExpressionMatch match{asctime.match(headerValue)};
    if (!match.hasMatch()) {
        valid = false;
        return;
    }

    int month = months.indexOf(match.captured(1));
    if (month == -1) {
        valid = false;
        return;
    } else {
        ++month;
    }

    int day = match.captured(2).toInt();
    int hours = match.captured(3).toInt();
    int minutes = match.captured(4).toInt();
    int seconds = match.captured(5).toInt();
    int year = match.captured(6).toInt();

    dateTime = QDateTime(QDate(year, month, day),
                         QTime(hours, minutes, seconds),
                         Qt::UTC);
}

inline Asctime::operator bool()
{
    return valid;
}

inline QDateTime Asctime::operator ()()
{
    return dateTime;
}

} // namespace Tufao

#endif // TUFAO_PRIV_ASCTIME_H
