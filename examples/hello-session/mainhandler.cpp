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
#include <QtCore/QVariant>
#include <Tufao/HttpServerRequest>
#include <Tufao/Headers>
#include <Tufao/Url>
#include <Tufao/QueryString>
#include <Tufao/SimpleSessionStore>

#define MANUAL "You can choose an action appending ?action=foobar to the\n" \
    "url, where foobar is the action you want to perform\n" \
    "\n" \
    "Available actions are:\n" \
    "  - set: Set the cookie's \"what\" property to the current url path\n" \
    "    component\n" \
    "  - view: Show cookie's state and \"what\" property\n" \
    "  - unset: Unset the cookie, if any"

MainHandler::MainHandler(QObject *parent) :
    QObject(parent)
{
}

void MainHandler::handleRequest(Tufao::HttpServerRequest *request,
                                Tufao::HttpServerResponse *response)
{
    Tufao::Url url(request->url());
    QMap<QByteArray, QByteArray> vars(Tufao::QueryString
                                      ::parse(url.query().toUtf8()));
    Tufao::SessionStore &store(Tufao::SimpleSessionStore::defaultInstance());

    if (vars["action"] == "unset") {
        store.removeSession(*request, *response);
        response->writeHead(Tufao::HttpServerResponse::OK);
        response->headers().replace("Content-Type", "text/plain");
        response->end("Success!");
        return;
    } else if (vars["action"] == "set") {
        store.setProperty(*request, *response, "what", url.path());
        response->writeHead(Tufao::HttpServerResponse::OK);
        response->headers().replace("Content-Type", "text/plain");
        response->end("Success!");
    } else if (vars["action"] == "view") {
        response->writeHead(Tufao::HttpServerResponse::OK);
        response->headers().replace("Content-Type", "text/plain");

        if (!store.hasSession(*request)) {
            response->end("Failure!");
            return;
        }

        response->end("session[\"what\"] = "
                      + store.property(*request, *response, "what")
                      .toByteArray());
        return;
    }

    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "text/plain");
    response->end(MANUAL);
}
