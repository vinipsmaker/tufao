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

const QRegExp Rfc1036::rfc1036("(?:\\w{3}\\s*,\\s*)" // day
                               "(\\d{1,2})\\s+" // day-1
                               "(\\w{3})\\s+" // month-2
                               "(\\d{2})\\s+" // year-3
                               "(\\d{2}):" // hour-4
                               "(\\d{2}):" // minutes-5
                               "(\\d{2})\\s+" // seconds-6
                               "GMT"
                               );

} // namespace Tufao
