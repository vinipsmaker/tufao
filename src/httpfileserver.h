/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#ifndef TUFAO_HTTPFILESERVER_H
#define TUFAO_HTTPFILESERVER_H

#include "abstracthttpserverrequesthandler.h"
#include "headers.h"
#include "httpserverresponse.h"

class QFileInfo;

namespace Tufao {

namespace Priv {

struct HttpFileServer;

} // namespace Priv

/*!
  You can use this class to server static files under Tufão.

  There are two main approaches:
    - Construct an object and use the AbstractHttpServerRequestHandler API
    - Use the static methods to serve a file (or set the entity in the response
      body)

  The algorithm used to serve files will handle the following set of headers:
    - If-Modified-Since
    - If-Unmodified-Since
    - If-Range
    - Range

  It won't handle:
    - ETag (If-Match and If-None-Match headers)
    - Cache-Control response header: Useful for set cache max age
    - Content-Disposition response header
    - Content-MD5 response header

  Still, this class provides a robust HTTP file server, supporting conditional
  and byte-ranges requests.
  */
class TUFAO_EXPORT HttpFileServer: public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    explicit HttpFileServer(const QString dir, QObject *parent = 0);
    ~HttpFileServer();

    void setDir(const QString &dir);
    QString dir() const;

    Headers &customHeaders();

    /*!
      Analyze the \p request and serve the file pointed by \p filename.
      */
    static void serveFile(const QString &fileName, HttpServerRequest *request,
                          HttpServerResponse *response);

    /*!
      This member function doesn't serve any file, just set the response body to
      the file pointed by \p filename. It's useful in some scenarios, like
      404-pages.
      */
    static bool serveFile(const QString &fileName, HttpServerResponse *response,
                          int statusCode);

    static qint64 bufferSize();
    static void setBufferSize(qint64 size);

public slots:
    /*!
      It handles the request search and serving the desired file, if the file is
      found.

      \note
      This method won't let requests access files outside the root dir folder
      and should be prefered over self-made implementations.
      */
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList & = QStringList());

private:
    Priv::HttpFileServer *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPFILESERVER_H
