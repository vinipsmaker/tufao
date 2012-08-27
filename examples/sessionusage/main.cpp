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

#include <QtCore/QCoreApplication>

#include <Tufao/HttpServer>

#include <Tufao/HttpServerRequestRouter>
//#include <Tufao/HttpFileServer>
#include <Tufao/SimpleSessionStore>

#include "roothandler.h"
#include "notfound.h"
#include "sethandler.h"
#include "readhandler.h"
#include "unsethandler.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(static);
    QCoreApplication a(argc, argv);
    Tufao::HttpServer server;

    Tufao::HttpServerRequestRouter router;

//    Tufao::HttpFileServer fileServer("public");
    RootHandler rootHandler;
    NotFoundHandler handler404;

    SetHandler setHandler;
    ReadHandler readHandler;
    UnsetHandler unsetHandler;

    router.map(QRegExp("^/$"), &rootHandler)
            .map(QRegExp("^/set(?:/(\\w*))?$"), &setHandler)
            .map(QRegExp("^/read(?:/(\\w*))?$"), &readHandler)
            .map(QRegExp("^/unset(?:/(\\w*))?$"), &unsetHandler)
            .map(QRegExp(""), &handler404);

    // We set the router as the global request handler
    QObject::connect(&server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)),
                     &router, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

    // Last, we run our server
    server.listen(QHostAddress::Any, 8080);

    return a.exec();
}
