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

#ifndef TUFAO_SESSIONSTORE_H
#define TUFAO_SESSIONSTORE_H

#include "httpserverrequest.h"
#include "sessionsettings.h"

namespace Tufao {

/*!
 * \note You may set this session in any response (including 400-level and
 * 500-level status), but user agents may ignore sessions in 100-level
 * status.
 */
class TUFAO_EXPORT SessionStore : public QObject
{
    Q_OBJECT
public:
    explicit SessionStore(const SessionSettings &settings = defaultSettings(),
                          QObject *parent = 0);

    virtual bool hasSession(const HttpServerRequest &request) const = 0;
    virtual void removeSession(const HttpServerRequest &request,
                               HttpServerResponse &response) = 0;

    virtual QList<QByteArray> properties(const HttpServerRequest &request,
                                         const HttpServerResponse &response)
        const = 0;
    virtual bool hasProperty(const HttpServerRequest &request,
                             const HttpServerResponse &response,
                             const QByteArray &key) const = 0;
    virtual QVariant property(const HttpServerRequest &request,
                              HttpServerResponse &response,
                              const QByteArray &key) const = 0;
    virtual void setProperty(const HttpServerRequest &request,
                             HttpServerResponse &response,
                             const QByteArray &key, const QVariant &value) = 0;
    virtual void removeProperty(const HttpServerRequest &request,
                                HttpServerResponse &response,
                                const QByteArray &key) = 0;

    static SessionSettings defaultSettings();

protected:
    QByteArray session(const HttpServerRequest &request) const;
    QByteArray session(const HttpServerRequest &request,
                       const HttpServerResponse &response) const;
    void setSession(HttpServerResponse &response, const QByteArray &session)
        const;
    void unsetSession(HttpServerResponse &response) const;

    SessionSettings settings;
};

} // namespace Tufao

#endif // TUFAO_SESSIONSTORE_H
