/*  This file is part of the Tufão project
    Copyright (C) 2011 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "reasonphrase.h"
#include <QtCore/QMap>
#include <QtCore/QByteArray>

namespace Tufao {

struct Phrases
{
    Phrases();

    QMap<int, QByteArray> map;
};

inline Phrases::Phrases()
{
    // 1xx Informational
    map[100] = "Continue";
    map[101] = "Switching Protocols";
    map[102] = "Processing";
    map[103] = "Checkpoint";

    // 2xx Successful
    map[200] = "OK";
    map[201] = "Created";
    map[202] = "Accepted";
    map[203] = "Non-Authoritative Information";
    map[204] = "No Content";
    map[205] = "Reset Content";
    map[206] = "Partial Content";
    map[207] = "Multi-Status";
    map[208] = "Already Reported";
    map[226] = "IM Used";

    // 3xx Redirection
    map[300] = "Multiple Choices";
    map[301] = "Moved Permanently";
    map[302] = "Found";
    map[303] = "See Other";
    map[304] = "Not Modified";
    map[305] = "Use Proxy";
    map[306] = "Switch Proxy";
    map[307] = "Temporary Redirect";
    map[308] = "Resume Incomplete";

    // 4xx Client Error
    map[400] = "Bad Request";
    map[401] = "Unauthorized";
    map[402] = "Payment Required";
    map[403] = "Forbidden";
    map[404] = "Not Found";
    map[405] = "Method Not Allowed";
    map[406] = "Not Acceptable";
    map[407] = "Proxy Authentication Required";
    map[408] = "Request Timeout";
    map[409] = "Conflict";
    map[410] = "Gone";
    map[411] = "Length Required";
    map[412] = "Precondition Failed";
    map[413] = "Request Entity Too Large";
    map[414] = "Request-URI Too Long";
    map[415] = "Unsupported Media Type";
    map[416] = "Requested Range Not Satisfiable";
    map[417] = "Expectation Failed";
    map[418] = "I'm a teapot";
    map[422] = "Unprocessable Entity";
    map[423] = "Locked";
    map[424] = "Failed Dependency";
    map[425] = "Unordered Collection";
    map[426] = "Upgrade Required";
    map[428] = "Precondition Required";
    map[429] = "Too Many Requests";
    map[431] = "Request Header Fields Too Large";
    map[444] = "No Response";
    map[449] = "Retry With";
    map[499] = "Client Closed Request";

    // 5xx Internal Server Error
    map[500] = "Internal Server Error";
    map[501] = "Not Implemented";
    map[502] = "Bad Gateway";
    map[503] = "Service Unavailable";
    map[504] = "Gateway Timeout";
    map[505] = "HTTP Version Not Supported";
    map[506] = "Variant Also Negotiates";
    map[507] = "Insufficient Storage";
    map[508] = "Loop Detected";
    map[509] = "Bandwidth Limit Exceeded";
    map[510] = "Not Extended";
}

static Phrases phrases;

QByteArray reasonPhrase(int statusCode)
{
    if (!phrases.map.contains(statusCode))
        return QByteArray();

    return phrases.map[statusCode];
}

} // namespace Tufao
