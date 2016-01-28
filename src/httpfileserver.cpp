/*  This file is part of the Tufão project
    Copyright (C) 2012, 2013 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "priv/httpfileserver.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QPair>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QMimeDatabase>

#include <QtNetwork/QAbstractSocket>

#include "httpserverrequest.h"

static qint64 bufferSize = BUFFER_SIZE;
static const QMimeDatabase mimes;

inline static QList< QPair<qulonglong, qulonglong> >
ranges(const Tufao::Headers &headers, qulonglong fileSize)
{
    if (!headers.contains("Range"))
        return QList< QPair<qulonglong, qulonglong> >();

    QList< QPair<qulonglong, qulonglong> > ranges;

    QList<QByteArray> rangeHeaders(headers.values("Range"));
    foreach (QByteArray rangesSpecifier, rangeHeaders) {
        static const QByteArray bytesUnit("bytes=");
        if (!rangesSpecifier.startsWith(bytesUnit))
            continue;

        QList<QByteArray> rangeSet(rangesSpecifier.mid(bytesUnit.size())
                                   .split(','));
        foreach (QByteArray rangeSpec, rangeSet) {
            rangeSpec = rangeSpec.trimmed();

            if (rangeSpec.startsWith('-')) {
                bool ok;
                qulonglong nbytes = rangeSpec.mid(1).toULongLong(&ok);
                if (!ok || nbytes == 0)
                    continue;

                if (nbytes >= fileSize) {
                    ranges.push_back(QPair<qulonglong, qulonglong>
                                     (0, fileSize - 1));
                } else {
                    ranges.push_back(QPair<qulonglong, qulonglong>
                                     (fileSize - nbytes, fileSize - 1));
                }
            } else {
                int i = rangeSpec.indexOf('-');
                if (i == -1)
                    continue;

                bool ok[2];
                QPair<qulonglong, qulonglong>
                        range(rangeSpec.mid(0, i).toULongLong(&ok[0]),
                              rangeSpec.mid(1 + i).toULongLong(&ok[1]));

                if (!ok[1])
                    range.second = fileSize - 1;

                if (!ok[0] || range.second < range.first
                        || range.first >= fileSize)
                    continue;

                if (range.second >= fileSize)
                    range.second = fileSize-1;

                ranges.push_back(range);
            }
        }
    }

    return ranges;
}

namespace Tufao {

HttpFileServer::HttpFileServer(const QString &dir, QObject *parent) :
    QObject(parent),
    priv(new Priv(dir))
{
}

HttpFileServer::~HttpFileServer()
{
    delete priv;
}

void HttpFileServer::setDir(const QString &dir)
{
    priv->rootDir = dir;
}

QString HttpFileServer::dir() const
{
    return priv->rootDir;
}

void HttpFileServer::serveFile(const QString &fileName,
                               HttpServerRequest &request,
                               HttpServerResponse &response)
{
    // Check if method is alright
    {
        const QByteArray method(request.method());
        if (method != "GET" && method != "HEAD") {
            response.writeHead(HttpResponseStatus::METHOD_NOT_ALLOWED);
            response.headers().insert("Allow", "GET, HEAD");
            response.end();
            return;
        }
    }

    QFileInfo fileInfo(fileName);

    if (!fileInfo.exists()) {
        response.writeHead(HttpResponseStatus::NOT_FOUND);
        response.end();
        return;
    }

    // Check "If-Modified-Since" header
    if (request.headers().contains("If-Modified-Since")) {
        foreach (const QByteArray &value,
                 request.headers().values("If-Modified-Since")) {
            QDateTime date(Headers::toDateTime(value));

            // Ignore invalid values
            if (fileInfo.lastModified() < date || !date.isValid()) {
                continue;
            } else if (fileInfo.lastModified() == date) {
                response.writeHead(HttpResponseStatus::NOT_MODIFIED);
                response.end();
            }
        }
    }

    // Check "If-Unmodified-Since" header
    if (request.headers().contains("If-Unmodified-Since")) {
        foreach (const QByteArray &value,
                 request.headers().values("If-Unmodified-Since")) {
            QDateTime date(Headers::toDateTime(value));

            // Ignore invalid values
            if (fileInfo.lastModified() < date || !date.isValid()) {
                continue;
            } else if (fileInfo.lastModified() > date) {
                response.writeHead(HttpResponseStatus::PRECONDITION_FAILED);
                response.end();
            }
        }
    }

    // Check If-Range header
    if (request.headers().contains("If-Range")
            && request.headers().contains("Range")) {
        foreach (QByteArray value, request.headers().values("If-Range")) {
            // Ignore ETags
            int strSize = value.size();
            if ((strSize > 0 && value[0] == '"')
                    || (strSize > 1 && value[0] == 'W' && value[1] == '/')) {
                continue;
            } else {
                QDateTime date(Headers::toDateTime(value));

                // Ignore invalid values
                if (fileInfo.lastModified() < date || !date.isValid()) {
                    continue;
                } else if (fileInfo.lastModified() > date) {
                    // Return the entire entity using a 200 response
                    request.headers().remove("Range");
                    break;
                }
            }
        }
    }

    // All conditionals were okay, continue...

    response.headers().insert("Accept-Ranges", "bytes");
    response.headers().insert("Date", Headers
                              ::fromDateTime(QDateTime::currentDateTime()));
    response.headers().insert("Last-Modified", Headers
                              ::fromDateTime(fileInfo.lastModified()));

    {
        QByteArray mime = mimes.mimeTypeForFile(fileInfo).name().toUtf8();
        if (mime.size())
            response.headers().insert("Content-Type", mime);
    }

    if (request.method() == "HEAD") {
        response.writeHead(HttpResponseStatus::OK);
        response.end();
        return;
    }

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QList< QPair<qulonglong, qulonglong> >
            ranges(::ranges(request.headers(), fileInfo.size()));

    // Not a byterange request
    if (!ranges.size()) {
        // Not a _satisfiable_ byterange request
        if (request.headers().contains("Range")) {
            static const QByteArray bytesUnit("bytes */");

            response.writeHead(HttpResponseStatus
                               ::REQUESTED_RANGE_NOT_SATISFIABLE);
            response.headers().insert("Content-Range", bytesUnit
                                      + QByteArray::number(fileInfo.size()));
            response.end();
            return;
        }

        // Just send the entity
        response.writeHead(HttpResponseStatus::OK);

        while (!file.atEnd()) {
            response << file.read(::bufferSize);
            request.socket().flush();
        }

        response.end();
    } else if (ranges.size() == 1) {
        // ONE range
        static const QByteArray bytesUnit("bytes ");

        response.writeHead(HttpResponseStatus::OK);
        QPair<qulonglong, qulonglong> &range(ranges[0]);
        response.headers().insert("Content-Range", bytesUnit
                                  + QByteArray::number(range.first)
                                  + '-'
                                  + QByteArray::number(range.second)
                                  + '/'
                                  + QByteArray::number(fileInfo.size()));
        file.seek(range.first);

        qint64 remaining = 1 + range.second - range.first;
        while (remaining) {
            QByteArray chunk(file.read(qMin(remaining, ::bufferSize)));
            response << chunk;
            request.socket().flush();
            remaining -= chunk.size();
        }

        response.end();
    } else {
        // MULTIPLE ranges
        QByteArray contentType(response.headers().value("Content-Type"));
        response.headers().replace("Content-Type", "multipart/byteranges;"
                                   " boundary=THIS_STRING_SEPARATES");

        for (int i = 0;i != ranges.size();++i) {
            QPair<qulonglong, qulonglong> &range(ranges[i]);
            response << "--THIS_STRING_SEPARATES\r\n";
            if (!contentType.isEmpty())
                response << "Content-type: " << contentType << "\r\n";

            response << QByteArray("Content-range: bytes")
                     << QByteArray::number(range.first) << QByteArray(1, '-')
                     << QByteArray::number(range.second)
                     << QByteArray(1, '/')
                     << QByteArray::number(fileInfo.size()) << "\r\n"
                     << QByteArray("\r\n");

            file.seek(range.first);

            qint64 remaining = 1 + range.second - range.first;
            while (remaining) {
                QByteArray chunk(file.read(qMin(remaining, ::bufferSize)));
                response << chunk;
                request.socket().flush();
                remaining -= chunk.size();
            }
        }
        response.end("--THIS_STRING_SEPARATES--\r\n");
    }
}

bool HttpFileServer::serveFile(const QString &fileName,
                               HttpServerResponse &response,
                               HttpResponseStatus statusCode)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    response.writeHead(statusCode);

    qint64 remaining = file.size();
    while (remaining) {
        QByteArray chunk(file.read(qMin(remaining, ::bufferSize)));
        response << chunk;
        response.flush();
        remaining -= chunk.size();
    }
    response.end();

    return true;
}

qint64 HttpFileServer::bufferSize()
{
    return ::bufferSize;
}

void HttpFileServer::setBufferSize(qint64 size)
{
    if (size <= 0)
        return;

    ::bufferSize = size;
}

std::function<bool(HttpServerRequest&, HttpServerResponse&)>
HttpFileServer::handler(const QString &rootDir)
{
    QString dir = rootDir;
    if (dir.endsWith(QDir::separator()))
        dir.remove(dir.size() - 1, 1);

    return [dir](HttpServerRequest &request, HttpServerResponse &response) {
        return handleRequest(request, response, dir);
    };
}

bool HttpFileServer::handleRequest(HttpServerRequest &request,
                                   HttpServerResponse &response)
{
    return handleRequest(request, response, priv->rootDir);
}

inline static QString filenameFromRequest(const HttpServerRequest &request,
                                          const QString &rootDir)
{
    return QString{
        QDir::cleanPath(rootDir
                        + QDir::toNativeSeparators(request.url()
                                                   .path(QUrl::FullyDecoded)))};
}

bool HttpFileServer::canHandleRequest(const HttpServerRequest &request)
{
    return canHandleRequest(request, priv->rootDir);
}

bool HttpFileServer::canHandleRequest(const HttpServerRequest &request,
                                      const QString &rootDir)
{
    QString fileName = filenameFromRequest(request, rootDir);
    if (!fileName.startsWith(rootDir + "/"))
        return false;

    {
        QFileInfo fileInfo(fileName);
        if (!fileInfo.exists() || !fileInfo.isFile())
            return false;
    }

    return true;
}

inline bool HttpFileServer::handleRequest(HttpServerRequest &request,
                                          HttpServerResponse &response,
                                          const QString &rootDir)
{
    if (!canHandleRequest(request, rootDir))
        return false;

    QString fileName = filenameFromRequest(request, rootDir);
    serveFile(fileName, request, response);
    return true;
}

} // namespace Tufao
