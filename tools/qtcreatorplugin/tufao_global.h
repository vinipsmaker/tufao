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

#ifndef TUFAO_GLOBAL_H
#define TUFAO_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(TUFAO_LIBRARY)
#  define TUFAO_EXPORT Q_DECL_EXPORT
#else
#  define TUFAO_EXPORT Q_DECL_IMPORT
#endif

#endif // TUFAO_GLOBAL_H
