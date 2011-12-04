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

#ifndef TUFAO_PRIV_CASEINSENSITIVEBYTEARRAYMAP_H
#define TUFAO_PRIV_CASEINSENSITIVEBYTEARRAYMAP_H

#include <QMap>
#include <QByteArray>

namespace Tufao {
namespace Priv {

struct CaseInsensitiveByteArrayMap
{
    bool contains(const QByteArray &key) const;
    bool is(const QByteArray &key, const QByteArray &value);

    QByteArray &operator [](const QByteArray &key);
    void remove(const QByteArray &key);
    void clear();

    QMap<QByteArray, QByteArray> map;
};

inline bool CaseInsensitiveByteArrayMap::contains(const QByteArray &key) const
{
    return map.contains(key.toLower());
}

inline bool CaseInsensitiveByteArrayMap::is(const QByteArray &key,
                                            const QByteArray &value)
{
    return contains(key) && (*this)[key] == value;
}

inline QByteArray &CaseInsensitiveByteArrayMap::operator [](const QByteArray
                                                            &key)
{
    return map[key.toLower()];
}

inline void CaseInsensitiveByteArrayMap::remove(const QByteArray &key)
{
    map.remove(key.toLower());
}

inline void CaseInsensitiveByteArrayMap::clear()
{
    map.clear();
}

} // namespace Priv
} // namespace Tufao

#endif // TUFAO_PRIV_CASEINSENSITIVEBYTEARRAYMAP_H
