/*
  Copyright (c) 2012, 2013 Vinícius dos Santos Oliveira

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

#ifndef TUFAO_HTTPFILESERVER_H
#define TUFAO_HTTPFILESERVER_H

#include "abstracthttpserverrequesthandler.h"
#include "headers.h"
#include "httpserverresponse.h"

class QFileInfo;

namespace Tufao {

/*!
  You can use this class to serve static files under Tufão. It provides a robust
  HTTP file server, supporting conditional and byte-range requests.

  The two main approaches are:
    - Construct an object and use the AbstractHttpServerRequestHandler API
      implemented by HttpFileServer
    - Use the static methods to serve a file (or set the entity in the response
      body)

  The algorithm used to serve files will handle the following set of headers:
    - If-Modified-Since
    - If-Unmodified-Since
    - If-Range
    - Range
    - Content-Type through QMimeDatabase (_Since version 1.0_)

  It won't handle:
    - ETag (If-Match and If-None-Match headers)
    - Cache-Control response header: Useful for set cache max age
    - Content-Disposition response header
    - Content-MD5 response header

  \since
  0.3
  */
class TUFAO_EXPORT HttpFileServer: public QObject,
                                   public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a HttpFileServer object.

      \p parent is passed to the QObject constructor.

      \p dir is used as root dir to serve files.
      */
    explicit HttpFileServer(const QString &dir, QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpFileServer();

    /*!
      Set the root dir.

      The root dir is the dir containing the files to be served by the
      HttpFileServer object.
      */
    void setDir(const QString &dir);

    /*!
      Return the root dir containing the files to be served.
      */
    QString dir() const;

    /*!
      Analyze the \p request and serve the file pointed by \p filename.

      \since
      1.0
      */
    static void serveFile(const QString &fileName, HttpServerRequest &request,
                          HttpServerResponse &response);

    /*!
      This member function doesn't serve any file, just set the response body to
      the contents in the file pointed by \p filename. It's useful in some
      scenarios, like serving 404-pages.

      \since
      1.0
      */
    static bool serveFile(const QString &fileName, HttpServerResponse &response,
                          HttpResponseStatus statusCode);

    /*!
      Return the buffer size used.

      When serving files, HttpFileServer allocates some bytes of the file in the
      memory before sending it to the network. The maximum space allocated is
      the buffer size. This method returns what number is this.

      \note
      The buffer size is global to all HttpFileServer objects.

      \sa
      setBufferSize
      */
    static qint64 bufferSize();

    /*!
      Set the buffer size.
      */
    static void setBufferSize(qint64 size);

    /*!
      Returns true iff HttpFileServer::handleRequest will return true.

      \since
      1.3
      */
    bool canHandleRequest(const HttpServerRequest &request);

    /*!
      Returns true iff HttpFileServer::handleRequest will return true.

      \since
      1.3
      */
    static bool canHandleRequest(const HttpServerRequest &request,
                                 const QString &root);

    /*!
      Returns a handler that don't depends on another object. The purpose of
      this alternative handler is to free you of the worry of maintain the
      HttpFileServer's object (lifetime) while the functor object is being used.

      \param rootDir The root dir to serve files.

      \since
      1.0
     */
    static std::function<bool(HttpServerRequest&, HttpServerResponse&)>
    handler(const QString &rootDir);

public slots:
    /*!
      It searchs for the file requested in the root dir and respond to the
      request, if the file is found.

      \note
      This method won't let requests access files outside the root dir folder
      and should be prefered over self-made implementations, as its safer.

      \since
      1.0
      */
    bool handleRequest(Tufao::HttpServerRequest &request,
                       Tufao::HttpServerResponse &response) override;

private:
    static bool handleRequest(HttpServerRequest &request,
                              HttpServerResponse &response,
                              const QString &rootDir);

    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPFILESERVER_H
