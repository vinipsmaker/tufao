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

/*!
 * \def DEFAULT_REFRESH_INTERVAL
 * This macro represents the default refresh interval of SimpleSessionStore. The
 * value is represented in miliseconds.
 */
#define DEFAULT_REFRESH_INTERVAL 2 * 60 * 1000

namespace Tufao {

/*!
 * SimpleSessionStore implements a simple storage mechanism to SessionStore. It
 * will store the data in the system memory.
 *
 * It will look for expired cookies (and delete them) at a defined interval (the
 * default value is DEFAULT_REFRESH_INTERVAL).
 *
 * \since 0.4
 */
class TUFAO_EXPORT SimpleSessionStore : public SessionStore
{
    Q_OBJECT
public:
    /*!
     * Constructs a new SimpleSessionStore object.
     *
     * It will pass \p parent to QObject constructor and \p settings to
     * SessionStore constructor.
     */
    explicit SimpleSessionStore(const SessionSettings &
                                settings = defaultSettings(),
                                QObject *parent = 0);

    /*!
     * Destructs a SimpleSessionStore object.
     */
    ~SimpleSessionStore();

    /*!
     * The refresh interval used to look for (and delete) expired cookies.
     *
     * The default interval is the value of the macro DEFAULT_REFRESH_INTERVAL.
     */
    int refreshInterval() const;

    /*!
     * Sets the refresh interval used to look for (and delete) expired cookies.
     */
    void setRefreshInterval(int msecs);

    /*!
     * Implements SessionStore::hasSession.
     */
    bool hasSession(const HttpServerRequest &request) const override;

    /*!
     * Implements SessionStore::removeSession.
     */
    void removeSession(const HttpServerRequest &request,
                       HttpServerResponse &response) override;

    /*!
     * Implements SessionStore::properties.
     */
    QList<QByteArray> properties(const HttpServerRequest &request,
                                 const HttpServerResponse &response)
    const override;

    /*!
     * Implements SessionStore::hasProperty.
     */
    bool hasProperty(const HttpServerRequest &request,
                     const HttpServerResponse &response,
                     const QByteArray &key) const override;

    /*!
     * Implements SessionStore::property
     */
    QVariant property(const HttpServerRequest &request,
                      HttpServerResponse &response,
                      const QByteArray &key) const override;

    /*!
     * Implements SessionStore::setProperty.
     */
    void setProperty(const HttpServerRequest &request,
                     HttpServerResponse &response, const QByteArray &key,
                     const QVariant &value) override;

    /*!
     * Implements SessionStore::removeProperty.
     */
    void removeProperty(const HttpServerRequest &request,
                        HttpServerResponse &response,
                        const QByteArray &key) override;

    /*!
     * Returns a reference to the same store every time it's called. It acts
     * like a [singleton](https://en.wikipedia.org/wiki/Singleton_pattern).
     *
     * \note
     * This method is added for convenience and if you are developing
     * multithreaded applications you shouldn't use it. But, if you insist use
     * it for these applications, you can use a mutex to control its access.
     */
    static SimpleSessionStore &defaultInstance();

private slots:
    void onTimer();

private:
    QByteArray createSession() const;

    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_SIMPLESESSIONSTORE_H
