/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "priv/httpserverrequest.h"
#include <QUrl>
#include <QBuffer>

namespace Tufao {

const http_parser_settings HttpServerRequest::Priv::httpSettingsInstance
= HttpServerRequest::Priv::httpSettings();

HttpServerRequest::HttpServerRequest(QAbstractSocket *socket, QObject *parent) :
    QObject(parent),
    priv(new Priv(this, socket))
{
    if (!socket)
        return;

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SIGNAL(close()));

    connect(&priv->timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    if (priv->timeout)
        priv->timer.start(priv->timeout);
}

HttpServerRequest::~HttpServerRequest()
{
    clearRequest();
    delete priv;
}

QByteArray HttpServerRequest::method() const
{
    return priv->method;
}

QByteArray HttpServerRequest::url() const
{
    return priv->url;
}

Headers HttpServerRequest::headers() const
{
    return priv->headers;
}

Headers &HttpServerRequest::headers()
{
    return priv->headers;
}

Headers HttpServerRequest::trailers() const
{
    return priv->trailers;
}

HttpServerRequest::HttpVersion HttpServerRequest::httpVersion() const
{
    return priv->httpVersion;
}

Parameters HttpServerRequest::parameters() const
{
    return priv->parameters;
}

UploadedFiles HttpServerRequest::uploadedFiles() const
{
    return priv->uploadedFiles;
}

QByteArray HttpServerRequest::body() const
{
    return priv->body;
}

QAbstractSocket *HttpServerRequest::socket() const
{
    return priv->socket;
}

void HttpServerRequest::setTimeout(int msecs)
{
    priv->timeout = msecs;

    if (priv->timeout)
        priv->timer.start(priv->timeout);
    else
        priv->timer.stop();
}

int HttpServerRequest::timeout() const
{
    return priv->timeout;
}

HttpServerResponse::Options HttpServerRequest::responseOptions() const
{
    return priv->responseOptions;
}

void HttpServerRequest::onReadyRead()
{
    if (priv->timeout)
        priv->timer.start(priv->timeout);

    priv->buffer += priv->socket->readAll();
    size_t nparsed = http_parser_execute(&priv->parser,
                                         &Priv::httpSettingsInstance,
                                         priv->buffer.constData(),
                                         priv->buffer.size());

    if (priv->parser.http_errno) {
        priv->socket->close();
        return;
    }

    if (priv->whatEmit.testFlag(Priv::READY)) {
        priv->whatEmit &= ~Priv::Signals(Priv::READY);
        // TODO: remove on version 1.0
        emit ready(priv->responseOptions);
        emit ready();
    }

    if (priv->whatEmit.testFlag(Priv::DATA)) {
        priv->whatEmit &= ~Priv::Signals(Priv::DATA);
        QByteArray body(priv->body);
        priv->body.clear();
        emit data(body);
    }

    priv->buffer.remove(0, nparsed);

    if (priv->whatEmit.testFlag(Priv::END)) {
        priv->whatEmit &= ~Priv::Signals(Priv::END);
        emit end();
        return;
    }

    if (priv->parser.upgrade) {
        disconnect(priv->socket, SIGNAL(readyRead()),
                   this, SLOT(onReadyRead()));
        disconnect(priv->socket, SIGNAL(disconnected()), this, SIGNAL(close()));
        disconnect(&priv->timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

        QByteArray b(priv->buffer);
        clearBuffer();
        emit upgrade(b);
    }
}

void HttpServerRequest::onTimeout()
{
    priv->socket->close();
}

inline void HttpServerRequest::clearBuffer()
{
    priv->buffer.clear();
    priv->lastHeader.clear();
    priv->lastWasValue = true;
    priv->useTrailers = false;
}

inline void HttpServerRequest::clearRequest()
{
    priv->method.clear();
    priv->url.clear();
    priv->headers.clear();
    priv->trailers.clear();
    priv->parameters.clear();

    foreach (QTemporaryFile* file, priv->uploadedFiles.values()) {
        file->close();
        delete file;
    }
    priv->uploadedFiles.clear();
}

http_parser_settings HttpServerRequest::Priv::httpSettings()
{
    http_parser_settings settings;

    settings.on_message_begin
            = Tufao::HttpServerRequest::Priv::on_message_begin;
    settings.on_url = Tufao::HttpServerRequest::Priv::on_url;
    settings.on_header_field = Tufao::HttpServerRequest::Priv::on_header_field;
    settings.on_header_value = Tufao::HttpServerRequest::Priv::on_header_value;
    settings.on_headers_complete
            = Tufao::HttpServerRequest::Priv::on_headers_complete;
    settings.on_body = Tufao::HttpServerRequest::Priv::on_body;
    settings.on_message_complete
            = Tufao::HttpServerRequest::Priv::on_message_complete;

    return settings;
}

int HttpServerRequest::Priv::on_message_begin(http_parser *parser)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    request->clearRequest();
    return 0;
}

int HttpServerRequest::Priv::on_url(http_parser *parser, const char *at,
                                    size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    request->priv->url.append(at, length);
    return 0;
}

int HttpServerRequest::Priv::on_header_field(http_parser *parser, const char *at,
                                             size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    if (request->priv->lastWasValue) {
        request->priv->lastHeader = QByteArray(at, length);
        request->priv->lastWasValue = false;
    } else {
        request->priv->lastHeader.append(at, length);
    }
    return 0;
}

int HttpServerRequest::Priv::on_header_value(http_parser *parser, const char *at,
                                             size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    if (request->priv->lastWasValue) {
        if (request->priv->useTrailers) {
            request->priv->trailers
                    .replace(request->priv->lastHeader,
                             request->priv->trailers.value(request->priv
                                                           ->lastHeader)
                             + QByteArray(at, length));
        } else {
            request->priv->headers
                    .replace(request->priv->lastHeader,
                             request->priv->headers.value(request->priv
                                                          ->lastHeader)
                             + QByteArray(at, length));
        }
    } else {
        if (request->priv->useTrailers) {
            request->priv->trailers
                    .insert(request->priv->lastHeader, QByteArray(at, length));
        } else {
            request->priv->headers
                    .insert(request->priv->lastHeader, QByteArray(at, length));
        }
        request->priv->lastWasValue = true;
    }
    return 0;
}

int HttpServerRequest::Priv::on_headers_complete(http_parser *parser)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);

    request->priv->lastHeader.clear();
    request->priv->lastWasValue = true;
    request->priv->useTrailers = true;

    {
#define M(str) {(str), (sizeof(str) - 1)}
        static const struct {
            char str[12];
            int size;
        } methods[] =
        {
            M("DELETE"),
            M("GET"),
            M("HEAD"),
            M("POST"),
            M("PUT"),
            M("CONNECT"),
            M("OPTIONS"),
            M("TRACE"),
            M("COPY"),
            M("LOCK"),
            M("MKCOL"),
            M("MOVE"),
            M("PROPFIND"),
            M("PROPPATCH"),
            M("SEARCH"),
            M("UNLOCK"),
            M("REPORT"),
            M("MKACTIVITY"),
            M("CHECKOUT"),
            M("MERGE"),
            M("M-SEARCH"),
            M("NOTIFY"),
            M("SUBSCRIBE"),
            M("UNSUBSCRIBE"),
            M("PATCH"),
            M("PURGE")
        };
#undef M

#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
        request->priv->method
                = QByteArray::fromRawData(methods[parser->method].str,
                                          methods[parser->method].size);
#else
        request->priv->method.setRawData(methods[parser->method].str,
                                         methods[parser->method].size);
#endif
    }

    {
        static const char errorMessage[]
                = "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                "Connection: close\r\n"
                "\r\n"
                "This server only offers support to HTTP/1.0 and HTTP/1.1\n";
        switch (parser->http_major) {
        case 1:
            switch (parser->http_minor) {
            case 0:
                request->priv->httpVersion = Tufao::HttpServerRequest::HTTP_1_0;
                break;
            case 1:
                request->priv->httpVersion = Tufao::HttpServerRequest::HTTP_1_1;
                break;
            default:
                request->priv->socket->write(errorMessage,
                                             sizeof(errorMessage) - 1);
                request->clearBuffer();
                request->clearRequest();
                return -1;
            }
            break;
        default:
            request->priv->socket->write(errorMessage,
                                         sizeof(errorMessage) - 1);
            request->clearBuffer();
            request->clearRequest();
            return -1;
        }
    }

    request->priv->responseOptions = 0;

    if (parser->http_minor == 1)
        request->priv->responseOptions |= HttpServerResponse::HTTP_1_1;
    else
        request->priv->responseOptions |= HttpServerResponse::HTTP_1_0;

    if (http_should_keep_alive(&request->priv->parser))
        request->priv->responseOptions |= HttpServerResponse::KEEP_ALIVE;

    if (!parser->upgrade)
        request->priv->whatEmit = READY;

    return 0;
}

int HttpServerRequest::Priv::on_body(http_parser *parser, const char *at,
                                     size_t length)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    request->priv->body.append(at, length);
    request->priv->whatEmit |= Priv::DATA;
    return 0;
}

static QByteArray urlDecode(QByteArray a_source) {
    a_source.replace('+', ' ');
    return QByteArray::fromPercentEncoding(a_source);
}

int HttpServerRequest::Priv::on_message_complete(http_parser *parser)
{
    Tufao::HttpServerRequest *request = static_cast<Tufao::HttpServerRequest *>
            (parser->data);
    Q_ASSERT(request);
    if (!parser->upgrade) {
        request->clearBuffer();
        request->priv->whatEmit |= Priv::END;
    }

    { // decode request params
        // url query params
        QPair<QByteArray, QByteArray> l_qry;
        foreach (l_qry, QUrl(request->url()).encodedQueryItems()) {
            request->priv->parameters.insert(urlDecode(l_qry.first),
                                             urlDecode(l_qry.second));
        }

        QString l_contentType = QString::fromUtf8(request->headers().value("Content-Type")).toLower();
        if (l_contentType == "application/x-www-form-urlencoded") {
            // Split the parameters into pairs of value and name
            foreach (QByteArray l_parm, request->priv->body.split('&')) {
                if (!l_parm.isEmpty()) {
                    int l_equalsChar = l_parm.indexOf('=');
                    if (l_equalsChar >= 0) {
                        request->priv->parameters.insert(urlDecode(l_parm.left(l_equalsChar).trimmed()),
                                                         urlDecode(l_parm.mid(l_equalsChar+1).trimmed()));
                    } else {
                        // Name without value
                        request->priv->parameters.insert(urlDecode(l_parm), "");
                    }
                }
            }
        } else
        if (l_contentType.startsWith("multipart/form-data")) {
            int pos = l_contentType.indexOf("boundary=");
            if (pos >= 0) {
                QByteArray boundary;
                QBuffer buffer(&request->priv->body);
                QByteArray paramName;
                QByteArray fileName;
                QTemporaryFile* uploadedFile = NULL;
                QByteArray paramValue;
                QByteArray l_line;

                boundary = request->headers().value("Content-Type").mid(pos + 9);

                buffer.open(QBuffer::ReadOnly);
                bool finished = false;
                while (!buffer.atEnd() && !finished) {
                    while (!buffer.atEnd() && !finished) {
                        QByteArray l_line = buffer.readLine(65536).trimmed();
                        if (l_line.startsWith("Content-Disposition:")) {
                            if (l_line.contains("form-data")) {
                                int l_start = l_line.indexOf(" name=\"");
                                int l_end = l_line.indexOf("\"", l_start + 7);
                                if (l_start >= 0 && l_end >= l_start) {
                                    paramName = l_line.mid(l_start + 7, l_end - l_start - 7);
                                }
                                l_start = l_line.indexOf(" filename=\"");
                                l_end = l_line.indexOf("\"", l_start + 11);
                                if (l_start >= 0 && l_end >= l_start) {
                                    fileName = l_line.mid(l_start + 11, l_end - l_start - 11);
                                }
                            } else {
                                qDebug("HttpRequest: ignoring unsupported content part %s", l_line.data());
                            }
                        } else
                        if (l_line.isEmpty()) {
                            break;
                        }
                    }

                    while (!buffer.atEnd() && !finished) {
                        l_line = buffer.readLine(65536);
                        if (l_line.startsWith("--" + boundary)) {
                            // Boundary found. Until now we have collected 2 bytes too much,
                            // so remove them from the last result
                            if (fileName.isEmpty() && !paramName.isEmpty()) {
                                // last param was a form param (remove \r\n)
                                request->priv->parameters.insert(paramName, paramValue.left(paramValue.size() - 2));
                            } else if (!fileName.isEmpty() && !paramName.isEmpty()) {
                                // last param was a file
                                uploadedFile->resize(uploadedFile->size() - 2);
                                uploadedFile->flush();
                                uploadedFile->seek(0);
                                request->priv->parameters.insert(paramName, fileName);
                                request->priv->uploadedFiles.insert(paramName, uploadedFile);
                                uploadedFile = NULL;
                            }
                            if (l_line.contains(boundary + "--")) {
                                finished = true;
                            }
                            break;
                        } else {
                            if (fileName.isEmpty() && !paramName.isEmpty()) {
                                // this is a form param.
                                paramValue.append(QString::fromUtf8(l_line));
                            } else
                            if (!fileName.isEmpty() && !paramName.isEmpty()) {
                                // this is a file
                                if (!uploadedFile) {
                                    uploadedFile = new QTemporaryFile();
                                    uploadedFile->open();
                                }
                                uploadedFile->write(l_line);
                                if (uploadedFile->error()) {
                                    qCritical("HttpRequest: error writing temp file, %s",qPrintable(uploadedFile->errorString()));
                                }
                            }
                        }
                    }
                    if (uploadedFile) {
                        delete uploadedFile;
                        uploadedFile = 0;
                    }
                }
            }
        }
    }
    return 0;
}

} // namespace Tufao
