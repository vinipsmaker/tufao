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

#ifndef TUFAO_ABSTRACTHTTPUPGRADEHANDLER_H
#define TUFAO_ABSTRACTHTTPUPGRADEHANDLER_H

#include "tufao_global.h"
#include <QtCore/QObject>
#include <QtCore/QStringList>

namespace Tufao {

class HttpServerRequest;

/*!
  This class provides an interface for HTTP upgrade handlers.

  \sa
  HttpUpgradeRouter

  \since
  0.6
  */
class TUFAO_EXPORT AbstractHttpUpgradeHandler: public QObject
{
    Q_OBJECT
public:
    /*!
      Constructs an AbstractHttpUpgradeHandler object.
      */
    explicit AbstractHttpUpgradeHandler(QObject *parent = 0);

public slots:
    /*!
      Handles the HTTP upgrade \p request.

      \param args A list of options that may be used by the handler. The
      HttpUpgradeRouter class uses this argument to send the captured texts in
      the regex pattern.

      \retval true
      If the handler wants to manage the connection from now on.

      \retval false
      If the url isn't interested in this \p request and the \p request should
      be transfered to somebody else.
      */
    virtual bool handleRequest(Tufao::HttpServerRequest *request,
                               const QByteArray &head,
                               const QStringList &args = QStringList()) = 0;
};

} // namespace Tufao

#endif // TUFAO_ABSTRACTHTTPUPGRADEHANDLER_H
