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

#include "mainhandler.h"
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>

MainHandler::MainHandler(QObject *parent) :
    QObject(parent)
{
}

void MainHandler::handleRequest(Tufao::HttpServerRequest *request,
                                Tufao::HttpServerResponse *response)
{
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "text/html; charset=utf-8");
    response->write("<html><head><title>Request dumper</title></head><body>");
    response->write("<p>Method: ");
    response->write(request->method());
    response->write("</p><p>Path: ");
    response->write(request->url());
    response->write("</p><p>Version: HTTP/1.");
    response->write(QByteArray(1, request->httpVersion() + '0'));
    response->write("</p><p>Headers:</p><ul>");
    {
        Tufao::Headers headers = request->headers();
        for (Tufao::Headers::const_iterator i = headers.begin()
             ;i != headers.end();++i) {
            response->write("<li>");
            response->write(i.key());
            response->write(": ");
            response->write(i.value());
        }
    }
    response->write("</ul>");
    response->end("</body></html>");
}
