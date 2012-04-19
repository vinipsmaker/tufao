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
      The two main differences between this static method and the object
      approaches are:
        - The object uses the Least-Recently-Used (LRU) cache algo
        - The object executes a safe (treay .. correctly) search for the file
          requested
      */
    static void serveFile(const QString &fileName, HttpServerRequest *request,
                          HttpServerResponse *response);

    static bool serveFile(const QString &fileName, HttpServerResponse *response,
                          int statusCode);

    static qint64 bufferSize();
    static void setBufferSize(qint64 size);

public slots:
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList & = QStringList());

private:
    Priv::HttpFileServer *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPFILESERVER_H
