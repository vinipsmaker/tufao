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
#include <QString>

namespace Tufao {

namespace Priv {

struct Url;

} // namespace Priv

/*!
  This class provides a convenient interface for parsing URLs.

  You should take the following url as reference for this document:
  \verbatim
scheme://userinfo@hostname:port/path?query#fragment
  \endverbatim

  Userinfo is compound of username and userpass in the form:
  \verbatim
username:userpass
  \endverbatim

  Some fields in URL are optionals. If the optional fields are not given to
  Tufao::Url constructor, the object will return empty strings for these fields.
  */
class TUFAO_EXPORT Url
{
public:
    /*!
      Constructs a Tufao::Url to extract information from \p url.
      */
    Url(const QString &url = QString());

    /*!
      Constructs a url as a copy of \p url.
      */
    Url(const Url &url);

    /*!
      Destroys the url.
      */
    ~Url();

    /*!
      Copies the url \p url and returns a reference to the copy.
      */
    Url &operator =(const Url &url);

    /*!
      The protocol.

      Examples:
        - "http"
        - "ftp"
      */
    QString scheme() const;

    /*!
      The authority.

      The authority is composed of:
        - userinfo (optional field)
        - hostname
        - port (optional field)

      \note userinfo and port are optionals.

      Examples:
        - "username:password@hostname:port"
        - "userinfo@hostname:port"
        - "example.com"
      */
    QString authority() const;

    /*!
      The path.

      Examples:
        - "/path"
        - "/index"
        - "/"
      */
    QString path() const;

    /*!
      The query string.

      Examples:
        - "querystring"
        - "type=penguin&name=tux"
      */
    QString query() const;

    /*!
      The hash.

      Examples:
        - "fragment"
      */
    QString fragment() const;

    /*!
      The userinfo.

      The userinfo usually is composed of username. Password is used sometimes
      also.

      \note It's usually unsafe to use password in url.

      Examples:
        - "username:password"
        - "cn=br;user=admin"
        - "tux"
      */
    QString userinfo() const;

    /*!
      The hostname.

      Examples:
        - "hostname"
        - "example.com"
      */
    QString hostname() const;

    /*!
      The port.

      Examples:
        - "80"
        - "443"
      */
    QString port() const;

    /*!
      The username.

      Examples:
        - "username"
        - "cn=br;user=admin"
        - "tux"
      */
    QString username() const;

    /*!
      The password.

      \warning It's usually unsafe to use password in url.
      */
    QString password() const;

private:
    Priv::Url *priv;
};

} // namespace Tufao

#endif // TUFAO_URL_H
