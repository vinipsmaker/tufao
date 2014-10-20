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

#include <QCoreApplication>
#include <QSharedPointer>

#include <Tufao/HttpFileServer>
#include <Tufao/HttpServerRequestRouter>
#include <Tufao/Headers>

#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TcpServer server;

    server.run(4, 8080, []() {
        QSharedPointer<Tufao::HttpServerRequestRouter> router;

        return [router](Tufao::HttpServerRequest &request,
                        Tufao::HttpServerResponse &response) mutable {
            if (!router) {
                router.reset(new Tufao::HttpServerRequestRouter);
                router->map({QRegularExpression{""},
                            Tufao::HttpFileServer::handler("public")});
            }

            if (!router->handleRequest(request, response)) {
                response.writeHead(Tufao::HttpResponseStatus::NOT_FOUND);
                response.headers().insert("Content-Type", "text/plain");
                response.end("Not found\n");
            }

            return true;
        };
    });

    return a.exec();
}
