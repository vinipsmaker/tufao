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

QByteArray stringify(const QMap<QByteArray, QByteArray> &map, char sep, char eq,
                     bool percentEncoding, char percent)
{
    QByteArray ret;

    if (percentEncoding) {
        for (QMap<QByteArray, QByteArray>::const_iterator i = map.begin()
             ;i != map.end();++i) {
            ret.append(i.key().toPercentEncoding(QByteArray(), QByteArray(),
                                                 percent)
                       + eq + i.value().toPercentEncoding(QByteArray(),
                                                          QByteArray(), percent)
                       + sep);
        }
    } else {
        for (QMap<QByteArray, QByteArray>::const_iterator i = map.begin()
             ;i != map.end();++i) {
            ret.append(i.key() + eq + i.value() + sep);
        }
    }

    if (map.size())
        ret.remove(ret.size() - 1, 1);

    return ret;
}

QMap<QByteArray, QByteArray> parse(const QByteArray &string, char sep, char eq,
                                   bool percentEncoding, char percent)
{
    QMap<QByteArray, QByteArray> ret;

    if (percentEncoding) {
        int i = 0;
        while (string[i] == '&') ++i;
        int j = string.indexOf(sep, i + 1);

        while (j != -1) {
            int k = string.indexOf(eq, i);

            if (k == -1 || k > j) {
                QByteArray key(string.mid(i, j - i));
                if (key.size())
                    ret[QByteArray::fromPercentEncoding(key, percent)];
            } else {
                QByteArray key(string.mid(i, k - i));
                if (key.size())
                    ret[QByteArray::fromPercentEncoding(key, percent)]
                            = QByteArray::fromPercentEncoding(string
                                                              .mid(k + 1,
                                                                   j - k - 1),
                                                              percent);
            }

            i = j + 1;
            j = string.indexOf(sep, j + 1);
        }

        int k = string.indexOf(eq, i);

        if (k == -1) {
            QByteArray key(string.mid(i, string.size() - i));
            if (key.size())
                ret[QByteArray::fromPercentEncoding(key, percent)];
        } else {
            QByteArray key(string.mid(i, k - i));
            if (key.size())
                ret[QByteArray::fromPercentEncoding(key, percent)]
                        = QByteArray::fromPercentEncoding(string
                                                          .mid(k + 1,
                                                               string.size()
                                                               - k - 1),
                                                          percent);
        }
    } else {
        int i = 0;
        while (string[i] == '&') ++i;
        int j = string.indexOf(sep, i + 1);

        while (j != -1) {
            int k = string.indexOf(eq, i);

            if (k == -1 || k > j) {
                QByteArray key(string.mid(i, j - i));
                if (key.size())
                    ret[key];
            } else {
                QByteArray key(string.mid(i, k - i));
                if (key.size())
                    ret[key] = string.mid(k + 1, j - k - 1);
            }

            i = j + 1;
            j = string.indexOf(sep, j + 1);
        }

        int k = string.indexOf(eq, i);

        if (k == -1) {
            QByteArray key(string.mid(i, string.size() - i));
            if (key.size())
                ret[key];
        } else {
            QByteArray key(string.mid(i, k - i));
            if (key.size())
                ret[key] = string.mid(k + 1, string.size() - k - 1);
        }
    }

    return ret;
}

} // namespace QueryString
} // namespace Tufao
