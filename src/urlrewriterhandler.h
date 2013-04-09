/*
  Copyright (c) 2013 Vinícius dos Santos Oliveira

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

#ifndef TUFAO_URLREWRITERHANDLER_H
#define TUFAO_URLREWRITERHANDLER_H

#include "abstracthttpserverrequesthandler.h"
#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Tufao {

/*!
  This class provides a handler to internally (only seen by your application)
  rewrite the URL.

  \note
  The handler does **NOT** redirects the HTTP client to another path. The new
  URL can only be seen by the Tufão application itself and HTTP clients will
  think they are receiving a response to the path they originally asked for.

  One place where this technique is useful is when you want to use one file to
  handle the root page of your site through HttpFileServer. This example is
  illustrated in the image below:

  ![](urlrewrite.png "Url rewrite example")

  \since
  0.6
  */
class TUFAO_EXPORT UrlRewriterHandler: public QObject,
                                       public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a UrlRewriterHandler object.

      \p parent is passed to the QObject constructor.

      \p url will be the new url.

      \since
      1.0
      */
    explicit UrlRewriterHandler(const QUrl &url, QObject *parent = 0);

    /*!
      Sets the URL that will replace the old URL to \p url.

      \since
      1.0
    */
    void setUrl(const QUrl &url);

    /*!
      Returns the url used to replace the old URLs.

      \since
      1.0
    */
    QUrl url() const;

    /*!
      Returns a handler that don't depends on another object. The purpose of
      this alternative handler is to free you of the worry of maintain the
      UrlRewriterHandler's object (lifetime) while the functor object is being
      used.

      \since
      1.0
     */
    static std::function<bool(HttpServerRequest&, HttpServerResponse&)>
    handler(const QUrl &url);

public slots:
    /*!
      Changes the \p request url and returns false. This method doesn't do
      anything else.

      \since
      1.0
      */
    bool handleRequest(Tufao::HttpServerRequest &request,
                       Tufao::HttpServerResponse &response) override;

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_URLREWRITERHANDLER_H
