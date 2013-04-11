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

#include "readhandler.h"

#include <Tufao/SimpleSessionStore>
#include <Tufao/Session>

#include <QtCore/QStringList>

ReadHandler::ReadHandler(QObject *parent) :
    QObject(parent)
{
}

bool ReadHandler::handleRequest(Tufao::HttpServerRequest &request,
                                Tufao::HttpServerResponse &response)
{
    response.writeHead(Tufao::HttpResponseStatus::OK);

    Tufao::SimpleSessionStore &store(Tufao::SimpleSessionStore
                                     ::defaultInstance());
    Tufao::Session session(store, request, response);

    QStringList args = request.customData().toMap()["args"].toStringList();
    const QByteArray property(args.isEmpty()
                              ? QByteArray()
                              : args[1].toUtf8());

    if (property.isEmpty()) {
        response << "You have the following properties:\n";

        QList<QByteArray> properties(store.properties(request, response));
        for (int i = 0;i != properties.size();++i)
            response << properties[i] + '\n';

        response.end();
        return true;
    }

    if (!session[property]) {
        response.end("You don't have a property named " + property);
        return true;
    }

    response.end("session[\"" + property + "\"] == "
                 + session[property]().toByteArray());
    return true;
}
