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
#include "httpserverresponse.h"

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
class TUFAO_EXPORT UrlRewriterHandler: public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a UrlRewriterHandler object.

      \p parent is passed to the QObject constructor.

      \p url will be the new url.
      */
    explicit UrlRewriterHandler(const QByteArray &url, QObject *parent = 0);

    /*!
      Sets the URL that will replace the old URL to \p url.
     */
    void setUrl(const QByteArray &url);

    /*!
      Returns the url used to replace the old URLs.
     */
    QByteArray url() const;

    /*!
      Destroys the object.
      */
    ~UrlRewriterHandler();

public slots:
    /*!
      Changes the \p request url and returns false. This method doesn't do
      anything else.
      */
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList & = QStringList());

private:
    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_URLREWRITERHANDLER_H
