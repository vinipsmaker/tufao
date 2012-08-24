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

#ifndef TUFAO_PRIV_CRYPTOGRAPHY_H
#define TUFAO_PRIV_CRYPTOGRAPHY_H

#include <QtCore/QCryptographicHash>

namespace Tufao {

inline QByteArray hmacSha1(QByteArray key, const QByteArray &message)
{
    const int blockSize = 64;
    const QCryptographicHash::Algorithm method(QCryptographicHash::Sha1);

    {
        int keySize = key.size();
        if (keySize > blockSize)
            key = QCryptographicHash::hash(key, method);
        else if (keySize < blockSize)
            key += QByteArray(blockSize - keySize, '\x00');
    }

    QByteArray oKeyPad(blockSize, '\x5c');
    QByteArray iKeyPad(blockSize, '\x36');

    for (int i = 0;i != blockSize;++i) {
        oKeyPad[i] = oKeyPad[i] ^ key[i];
        iKeyPad[i] = iKeyPad[i] ^ key[i];
    }

    return QCryptographicHash
        ::hash(oKeyPad + QCryptographicHash::hash(iKeyPad + message, method),
               method).toBase64();
}

} // namespace Tufao

#endif // TUFAO_PRIV_CRYPTOGRAPHY_H
