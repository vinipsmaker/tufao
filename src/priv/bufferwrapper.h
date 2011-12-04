/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#ifndef TUFAO_PRIV_BUFFERWRAPPER_H
#define TUFAO_PRIV_BUFFERWRAPPER_H

#include "../tufao_global.h"
#include <QByteArray>

namespace Tufao {
namespace Priv {

class BufferWrapper
{
public:
    BufferWrapper(QByteArray &buffer);

    bool line();
    QByteArray takeLine();

private:
    QByteArray &buffer;
    int i;
};

inline BufferWrapper::BufferWrapper(QByteArray &buffer) :
    buffer(buffer),
    i(-1)
{}

inline bool BufferWrapper::line()
{
    i = buffer.indexOf("\r\n");
    return i != -1;
}

inline QByteArray BufferWrapper::takeLine()
{
    if (i == -1)
        return QByteArray();

    QByteArray ret = buffer.left(i);
    buffer.remove(0, i + 2);
    return ret;
}

} // namespace Priv
} // namespace Tufao

#endif // TUFAO_PRIV_BUFFERWRAPPER_H
