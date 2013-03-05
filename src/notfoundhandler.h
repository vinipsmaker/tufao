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

#ifndef TUFAO_NOTFOUNDHANDLER_H
#define TUFAO_NOTFOUNDHANDLER_H

#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

/*!
  A handler that responds with "Not found" to every request.

  Its purpose is to avoid boilerplate code.

  \since
  1.0
  */
class TUFAO_EXPORT NotFoundHandler: public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a NotFoundHandler object.

      \p parent is passed to the QObject constructor.
      */
    explicit NotFoundHandler(QObject *parent = 0);

public slots:
    /*!
      It responds to the request with a not found message.
      */
    bool handleRequest(Tufao::HttpServerRequest &request,
                       Tufao::HttpServerResponse &response) override;
};

} // namespace Tufao

#endif // TUFAO_NOTFOUNDHANDLER_H
