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
  \brief
  This class provides a convenient interface for parsing URLs.

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

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the
      scheme will be "scheme".

      Other examples:
        - "http"
        - "ftp"
      */
    QString scheme() const;

    /*!
      The authority.

      The authority contains:
        - userinfo
        - hostname
        - port

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the
      authority will be "hostname:port".

      Other examples:
        - "username:password@hostname:port"
        - "userinfo@hostname:port"
        - "example.com"
      */
    QString authority() const;

    /*!
      The path.

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the path
      will be "/path".

      Other examples:
        - "/index"
        - "/"
      */
    QString path() const;

    /*!
      The query string.

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the query
      string will be "query".

      Other examples:
        - "type=penguin&name=tux"
        - "type=penguin&name=tux&age=20"
      */
    QString query() const;

    /*!
      The fragment id, also know as hash.

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the hash
      will be "fragment".

      \note The frament id is used by the user agents in the html and not sent
      to the browser, so this field may be always empty when reading from
      Tufao::HttpServerRequest.

      Other examples:
        - "title1"
        - "title1-1"
      */
    QString fragment() const;

    /*!
      The userinfo.

      The userinfo contains the username and password.

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the
      userinfo will be "userinfo".

      \note It's usually unsafe to use password in url.

      Other examples:
        - "username:password"
        - "cn=br;user=admin"
        - "tux"
      */
    QString userinfo() const;

    /*!
      The hostname.

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the
      hostname will be "hostname"

      Other examples:
        - "hostname"
        - "example.com"
        - "127.0.0.1"
      */
    QString hostname() const;

    /*!
      The port.

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the port
      will be "port".

      Other examples:
        - "80"
        - "443"
      */
    QString port() const;

    /*!
      The username.

      \warning This field is deprecated by the RFC and is here only to provide
      compatibility with old applications. You problably want use
      Tufao::Url::userinfo.

      This username is extracted from the userinfo assuming that userinfo
      uses the format "username:password".

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the
      username will be "userinfo".

      Other examples:
        - "username"
        - "cn=br;user=admin"
        - "tux"

      \sa
      Tufao::Url::userinfo
      Tufao::url::password
      */
    QString username() const;

    /*!
      The password.

      \warning This field is deprecated by the RFC and is here only to provide
      compatibility with old applications. It's usually unsafe to use passwords
      in urls and you problably don't want to use this technique.

      This password is extracted from the userinfo assuming that userinfo uses
      the format "username:password".

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the
      password will be "".

      Other examples:
        - "123456"
        - "42"

      \sa
      Tufao::Url::userinfo
      Tufao::Url::username
      */
    QString password() const;

private:
    Priv::Url *priv;
};

} // namespace Tufao

#endif // TUFAO_URL_H
