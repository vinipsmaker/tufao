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

#ifndef TUFAO_SIMPLESESSIONSTORE_H
#define TUFAO_SIMPLESESSIONSTORE_H

#include "sessionstore.h"

#define DEFAULT_REFRESH_INTERVAL 2 * 60 * 1000

namespace Tufao {

class TUFAO_EXPORT SimpleSessionStore : public SessionStore
{
    Q_OBJECT
public:
    explicit SimpleSessionStore(const SessionSettings &
                                settings = defaultSettings(),
                                QObject *parent = 0);
    ~SimpleSessionStore();

    /*!
     * Default interval is the value of the macro DEFAULT_REFRESH_INTERVAL.
     */
    int refreshInterval() const;
    void setRefreshInterval(int msecs);

    bool hasSession(const HttpServerRequest &request) const;
    void removeSession(const HttpServerRequest &request,
                       HttpServerResponse &response);

    QList<QByteArray> properties(const HttpServerRequest &request,
                                 const HttpServerResponse &response) const;
    bool hasProperty(const HttpServerRequest &request,
                     const HttpServerResponse &response,
                     const QByteArray &key) const;
    QVariant property(const HttpServerRequest &request,
                      HttpServerResponse &response,
                      const QByteArray &key) const;
    void setProperty(const HttpServerRequest &request,
                     HttpServerResponse &response, const QByteArray &key,
                     const QVariant &value);
    void removeProperty(const HttpServerRequest &request,
                        HttpServerResponse &response, const QByteArray &key);

private slots:
    void onTimer();

private:
    QByteArray createSession() const;

    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_SIMPLESESSIONSTORE_H
