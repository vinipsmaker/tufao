/*
  Copyright (c) 2012, 2013 Vinícius dos Santos Oliveira

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
#include <QUrl>
#include <Tufao/HttpServer>
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpFileServer>

using namespace Tufao;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    HttpServer server;
    HttpFileServer fileServer(":/");

    QObject::connect(&server, &HttpServer::requestReady,
                     [&fileServer](HttpServerRequest &req,
                     HttpServerResponse &res) {
        if (req.method() == "POST") {
            QObject::connect(&req, &HttpServerRequest::end, [&req,&res]() {
                res.writeHead(Tufao::HttpResponseStatus::OK);
                res.headers().insert("Content-Type", "text/plain");
                res.end(req.readBody());
            });
            return;
        }

        if (fileServer.handleRequest(req, res))
            return;

        res.writeHead(HttpResponseStatus::MOVED_PERMANENTLY);
        res.headers().insert("Location", "/index.html");
        res.end();
    });

    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
