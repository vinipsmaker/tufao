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

#ifndef TUFAO_SESSIONSETTINGS_H
#define TUFAO_SESSIONSETTINGS_H

#include <QtNetwork/QNetworkCookie>
#include <QtCore/QDateTime>

namespace Tufao {

struct SessionSettings
{
    QNetworkCookie cookie(const QByteArray &value = QByteArray()) const
    {
        return cookie(*this, value);
    }

    static QNetworkCookie cookie(const SessionSettings &settings,
                                 const QByteArray &value = QByteArray())
    {
        QNetworkCookie cookie;

        cookie.setExpirationDate(QDateTime::currentDateTimeUtc()
                                 .addSecs(settings.timeout * 60));
        cookie.setHttpOnly(settings.httpOnly);
        cookie.setName(settings.key);
        cookie.setPath(settings.path);
        cookie.setSecure(settings.secure);

        cookie.setValue(value);

        return cookie;
    }

    /*!
     * Timeout, in minutes.
     */
    int timeout;

    bool httpOnly;
    QByteArray key;
    QByteArray path;
    bool secure;
};

} // namespace Tufao

#endif // TUFAO_SESSION_H
