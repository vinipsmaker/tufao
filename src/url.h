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

#ifndef TUFAO_URL_H
#define TUFAO_URL_H

#include "tufao_global.h"

namespace Tufao {

/*!
  This class provides a convenient interface for parsing URLs.

  You should take the following url as reference:
  \verbatim
scheme://userinfo@hostname:port/path?query#fragment
  \endverbatim
  */
class TUFAO_EXPORT Url
{
public:
    /*!
      Constructs a empty URL.
      */
    Url();

    /*!
      Constructs a Tufao::Url parsing \p url.
      */
    Url(const QByteArray &url);

    /*!
      Returns the QByteArray representation of the URL.
      */
    operator QByteArray() const;

    /*!
      The protocol.

      Examples:
        - "http"
        - "ftp"
      */
    QByteArray scheme() const;

    /*!
      The authority.

      The authority is composed of:
        - userinfo
        - hostname
        - port

      Examples:
        - "username:password@hostname:port"
        - "userinfo@hostname:port"
        - "example.com"
      */
    QByteArray authority() const;

    /*!
      The path.

      Examples:
        - "/path"
        - "/index"
      */
    QByteArray path() const;

    /*!
      The query string.

      Examples:
        - "querystring"
        - "type=human&name=tux"
      */
    QByteArray query() const;

    /*!
      The hash.

      Examples:
        - "fragment"
      */
    QByteArray fragment() const;

    /*!
      The userinfo.

      The userinfo is composed of username and password.

      \note You shouldn't user password in url.

      Examples:
        - "username:password"
        - "cn=br;user=admin"
      */
    QByteArray userinfo() const;

    /*!
      The hostname.

      Examples:
        - "hostname"
        - "example.com"
      */
    QByteArray hostname() const;

    /*!
      The port.
      */
    QByteArray port() const;

    /*!
      The username.
      */
    QByteArray username() const;

    /*!
      The password.
      */
    QByteArray password() const;
};

} // namespace Tufao

#endif // TUFAO_URL_H
