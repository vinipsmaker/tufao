/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "querystring.h"
#include <QtCore/QByteArray>

namespace Tufao {
namespace QueryString {

inline QByteArray escape(const QByteArray &string, bool percentEncoding,
                         char percent)
{
    if (percentEncoding)
        return string.toPercentEncoding(QByteArray(), QByteArray(), percent);
    else
        return string;
}

QByteArray stringify(const QMap<QByteArray, QByteArray> &map, char sep, char eq,
                     bool percentEncoding, char percent)
{
    QByteArray ret;

    for (QMap<QByteArray, QByteArray>::const_iterator i = map.begin()
             ;i != map.end();++i) {
        ret.append(escape(i.key(), percentEncoding, percent) + eq
                   + escape(i.value(), percentEncoding, percent) + sep);
    }

    if (map.size())
        ret.remove(ret.size() - 1, 1);

    return ret;
}

inline QByteArray unescape(const QByteArray &string, bool percentEncoding,
                           char percent)
{
    QByteArray ret(percentEncoding
                   ? QByteArray::fromPercentEncoding(string, percent)
                   : string);
    ret.replace('+', ' ');
    return ret;
}

QMap<QByteArray, QByteArray> parse(const QByteArray &string, char sep, char eq,
                                   bool percentEncoding, char percent)
{
    QMap<QByteArray, QByteArray> ret;

    int i = 0;
    while (string[i] == '&') ++i;
    int j = string.indexOf(sep, i + 1);

    while (j != -1) {
        int k = string.indexOf(eq, i);

        if (k == -1 || k > j) {
            QByteArray key(string.mid(i, j - i));
            if (key.size())
                ret[unescape(key, percentEncoding, percent)];
        } else {
            QByteArray key(string.mid(i, k - i));
            if (key.size()) {
                ret[unescape(key, percentEncoding, percent)]
                    = unescape(string.mid(k + 1, j - k - 1), percentEncoding,
                               percent);
            }
        }

        i = j + 1;
        j = string.indexOf(sep, j + 1);
    }

    int k = string.indexOf(eq, i);

    if (k == -1) {
        QByteArray key(string.mid(i, string.size() - i));
        if (key.size())
            ret[unescape(key, percentEncoding, percent)];
    } else {
        QByteArray key(string.mid(i, k - i));
        if (key.size()) {
            ret[unescape(key, percentEncoding, percent)]
                = unescape(string .mid(k + 1, string.size() - k - 1),
                           percentEncoding, percent);
        }
    }

    return ret;
}

} // namespace QueryString
} // namespace Tufao
