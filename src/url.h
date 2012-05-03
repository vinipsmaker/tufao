/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

#ifndef TUFAO_URL_H
#define TUFAO_URL_H

#include "tufao_global.h"
#include <QtCore/QString>

namespace Tufao {

class HttpServerRequest;

/*!
  This class provides a convenient interface for parsing URLs.

  Some fields in URLs are optionals. If the optional fields are absent, the
  object will return empty strings for these fields, but if you try to use this
  class to parse an invalid url, you will get empty strings to all fields.

  \note
  Currently URL usage become commonplace and it's easy to find abbreviated URLs
  such as "www.example.com". These abbreviated URLs aren't valid and Tufao::Url
  won't parse them, but this isn't really a problem if you are using Tufao::Url
  just to parse urls received from Tufao::HttpServerRequest, because user agents
  implementing the HTTP protocol always send valid URLs.
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
      authority will be "userinfo@hostname:port".

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

      \sa
      Tufao::QueryString
      */
    QString query() const;

    /*!
      The fragment id, also know as hash.

      In the url "scheme://userinfo@hostname:port/path?query#fragment" the hash
      will be "fragment".

      \note
      The frament id is used by the user agents in the html and not sent to the
      browser, so this field may be always empty when reading from
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

      \note
      It's usually unsafe to use password in url.

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

      \warning
      This field is deprecated by the RFC and is here only to provide
      compatibility with old applications. You problably want to use
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
      Tufao::Url::password
      */
    QString username() const;

    /*!
      The password.

      \warning
      This field is deprecated by the RFC and is here only to provide
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

    /*!
      Returns the url likely used by the client to perform the \p request.

      \since 0.3
      */
    static QByteArray url(Tufao::HttpServerRequest *request);

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_URL_H
