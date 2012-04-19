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

#include "headers.h"
#include "priv/rfc1123.h"

namespace Tufao {

QByteArray Headers::fromDateTime(const QDateTime &dateTime)
{
    return dateTime.toUTC().toString(
                "ddd," // day
                " d MMM yyyy" // date
                " hh:mm:ss" // hour
                " GMT" // zone
                ).toUtf8();
}

QDateTime Headers::toDateTime(const QByteArray &headerValue)
{
    {
        Priv::Rfc1123 rfc1123(headerValue);
        if (rfc1123)
            return rfc1123();
    }
    // TODO:
    // rfc 1036/850
    // ANSI C's asctime
//    {
//    }
//    {
    //    }
}

} // namespace Tufao
