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

#include "unsethandler.h"

#include <Tufao/SimpleSessionStore>
#include <Tufao/Session>

UnsetHandler::UnsetHandler(QObject *parent) :
    Tufao::AbstractHttpServerRequestHandler(parent)
{
}

bool UnsetHandler::handleRequest(Tufao::HttpServerRequest *request,
                                 Tufao::HttpServerResponse *response,
                                 const QStringList &args)
{
    Tufao::SimpleSessionStore &store(Tufao::SimpleSessionStore
                                     ::defaultInstance());
    Tufao::Session session(store, *request, *response);

    const QByteArray property(args.isEmpty()
                              ? QByteArray()
                              : args.front().toUtf8());

    if (property.isEmpty())
        return false;

    response->writeHead(Tufao::HttpServerResponse::OK);

    if (!session[property]) {
        response->end("Property \"" + property + "\" doesn't exist");
        return true;
    }

    store.removeProperty(*request, *response, property);

    response->end("store.removeProperty(\"" + property + "\")");
    return true;
}
