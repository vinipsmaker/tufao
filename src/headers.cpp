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

#include "headers.h"
#include "priv/rfc1123.h"
#include "priv/rfc1036.h"
#include "priv/asctime.h"

#include <QtCore/QLocale>
#include <QtCore/QDebug>

namespace Tufao {

QByteArray Headers::fromDateTime(const QDateTime &dateTime)
{
    return QLocale(QLocale::C).toString(dateTime.toUTC(),
                "ddd," // day
                " dd MMM yyyy" // date
                " hh:mm:ss" // hour
                ).toUtf8() + " GMT"; // zone
}

QDateTime Headers::toDateTime(const QByteArray &headerValue,
                              const QDateTime &defaultValue)
{
    {
        Rfc1123 rfc1123(headerValue);
        if (rfc1123)
            return rfc1123();
    }
    {
        Rfc1036 rfc1036(headerValue);
        if (rfc1036)
            return rfc1036();
    }
    {
        Asctime asctime(headerValue);
        if (asctime)
            return asctime();
    }

    return defaultValue;
}

QDebug operator<<(QDebug dbg, const Headers &headers)
{
    for (Headers::const_iterator i = headers.begin(); i != headers.end();++i)
        dbg.nospace() << i.key() << ": " << i.value() << endl;

    return dbg.space();
}

} // namespace Tufao
