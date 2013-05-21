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

#include "rfc1036.h"

namespace Tufao {

// HTTP-date is case sensitive and MUST NOT include additional LWS beyond that
// specifically included as SP in the grammar. RFC 1036 updates and replaces
// RFC-850.

//   rfc850-date  = weekday "," SP date2 SP time SP "GMT"
//   weekday      = "Monday" | "Tuesday" | "Wednesday"
//              | "Thursday" | "Friday" | "Saturday" | "Sunday"
//   date2        = 2DIGIT "-" month "-" 2DIGIT
//   month        = "Jan" | "Feb" | "Mar" | "Apr"
//                | "May" | "Jun" | "Jul" | "Aug"
//                | "Sep" | "Oct" | "Nov" | "Dec"
//   time         = 2DIGIT ":" 2DIGIT ":" 2DIGIT

// Example: Sunday, 06-Nov-94 08:49:37 GMT

const QRegularExpression Rfc1036::rfc1036("\\w+,\\s" // day
                                          "(\\d{2})-" // day-1
                                          "(\\w{3})-" // month-2
                                          "(\\d{2})\\s" // year-3
                                          "(\\d{2}):" // hour-4
                                          "(\\d{2}):" // minutes-5
                                          "(\\d{2})\\s" // seconds-6
                                          "GMT");

const QStringList Rfc1036::months{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

} // namespace Tufao
