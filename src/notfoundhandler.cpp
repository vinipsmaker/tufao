/*  This file is part of the Tufão project
    Copyright (C) 2013 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "notfoundhandler.h"

namespace Tufao {

const QByteArray NotFoundHandler::body{"Not found"};

NotFoundHandler::NotFoundHandler(QObject *parent) :
    QObject(parent)
{
}

bool NotFoundHandler::handleRequest(HttpServerRequest &,
                                    HttpServerResponse &response)
{
    response.writeHead(HttpResponseStatus::NOT_FOUND);
    response.end(body);
    return true;
}

} // namespace Tufao
