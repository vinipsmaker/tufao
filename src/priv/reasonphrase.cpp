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
#include <QtCore/QByteArray>
#include <map>

namespace Tufao {

std::map<HttpResponseStatusCode, QByteArray> phrases {
    // 1xx Informational
    {HttpResponseStatusCode::CONTINUE, "Continue"},
    {HttpResponseStatusCode::SWITCHING_PROTOCOLS, "Switching Protocols"},
    {HttpResponseStatusCode::PROCESSING, "Processing"},
    {HttpResponseStatusCode::CHECKPOINT, "Checkpoint"},

    // 2xx Successful
    {HttpResponseStatusCode::OK, "OK"},
    {HttpResponseStatusCode::CREATED, "Created"},
    {HttpResponseStatusCode::ACCEPTED, "Accepted"},
    {HttpResponseStatusCode::NON_AUTHORITATIVE_INFORMATION,
                "Non-Authoritative Information"},
    {HttpResponseStatusCode::NO_CONTENT, "No Content"},
    {HttpResponseStatusCode::RESET_CONTENT, "Reset Content"},
    {HttpResponseStatusCode::PARTIAL_CONTENT, "Partial Content"},
    {HttpResponseStatusCode::MULTI_STATUS, "Multi-Status"},
    {HttpResponseStatusCode::ALREADY_REPORTED, "Already Reported"},
    {HttpResponseStatusCode::IM_USED, "IM Used"},

    // 3xx Redirection
    {HttpResponseStatusCode::MULTIPLE_CHOICES, "Multiple Choices"},
    {HttpResponseStatusCode::MOVED_PERMANENTLY, "Moved Permanently"},
    {HttpResponseStatusCode::FOUND, "Found"},
    {HttpResponseStatusCode::SEE_OTHER, "See Other"},
    {HttpResponseStatusCode::NOT_MODIFIED, "Not Modified"},
    {HttpResponseStatusCode::USE_PROXY, "Use Proxy"},
    {HttpResponseStatusCode::SWITCH_PROXY, "Switch Proxy"},
    {HttpResponseStatusCode::TEMPORARY_REDIRECT, "Temporary Redirect"},
    {HttpResponseStatusCode::RESUME_INCOMPLETE, "Resume Incomplete"},

    // 4xx Client Error
    {HttpResponseStatusCode::BAD_REQUEST, "Bad Request"},
    {HttpResponseStatusCode::UNAUTHORIZED, "Unauthorized"},
    {HttpResponseStatusCode::PAYMENT_REQUIRED, "Payment Required"},
    {HttpResponseStatusCode::FORBIDDEN, "Forbidden"},
    {HttpResponseStatusCode::NOT_FOUND, "Not Found"},
    {HttpResponseStatusCode::METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {HttpResponseStatusCode::NOT_ACCEPTABLE, "Not Acceptable"},
    {HttpResponseStatusCode::PROXY_AUTHENTICATION_REQUIRED,
                "Proxy Authentication Required"},
    {HttpResponseStatusCode::REQUEST_TIMEOUT, "Request Timeout"},
    {HttpResponseStatusCode::CONFLICT, "Conflict"},
    {HttpResponseStatusCode::GONE, "Gone"},
    {HttpResponseStatusCode::LENGTH_REQUIRED, "Length Required"},
    {HttpResponseStatusCode::PRECONDITION_FAILED, "Precondition Failed"},
    {HttpResponseStatusCode::REQUEST_ENTITY_TOO_LARGE,
                "Request Entity Too Large"},
    {HttpResponseStatusCode::REQUEST_URI_TOO_LONG, "Request-URI Too Long"},
    {HttpResponseStatusCode::UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"},
    {HttpResponseStatusCode::REQUESTED_RANGE_NOT_SATISFIABLE,
                "Requested Range Not Satisfiable"},
    {HttpResponseStatusCode::EXPECTATION_FAILED, "Expectation Failed"},
    {HttpResponseStatusCode::I_AM_A_TEAPOT, "I'm a teapot"},
    {HttpResponseStatusCode::UNPROCESSABLE_ENTITY, "Unprocessable Entity"},
    {HttpResponseStatusCode::LOCKED, "Locked"},
    {HttpResponseStatusCode::FAILED_DEPENDENCY, "Failed Dependency"},
    {HttpResponseStatusCode::UNORDERED_COLLECTION, "Unordered Collection"},
    {HttpResponseStatusCode::UPGRADE_REQUIRED, "Upgrade Required"},
    {HttpResponseStatusCode::PRECONDITION_REQUIRED, "Precondition Required"},
    {HttpResponseStatusCode::TOO_MANY_REQUESTS, "Too Many Requests"},
    {HttpResponseStatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE,
                "Request Header Fields Too Large"},
    {HttpResponseStatusCode::NO_RESPONSE, "No Response"},
    {HttpResponseStatusCode::RETRY_WITH, "Retry With"},
    {HttpResponseStatusCode::CLIENT_CLOSED_REQUEST, "Client Closed Request"},

    // 5xx Internal Server Error
    {HttpResponseStatusCode::INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {HttpResponseStatusCode::NOT_IMPLEMENTED, "Not Implemented"},
    {HttpResponseStatusCode::BAD_GATEWAY, "Bad Gateway"},
    {HttpResponseStatusCode::SERVICE_UNAVAILABLE, "Service Unavailable"},
    {HttpResponseStatusCode::GATEWAY_TIMEOUT, "Gateway Timeout"},
    {HttpResponseStatusCode::HTTP_VERSION_NOT_SUPPORTED,
                "HTTP Version Not Supported"},
    {HttpResponseStatusCode::VARIANT_ALSO_NEGOTIATES,
                "Variant Also Negotiates"},
    {HttpResponseStatusCode::INSUFFICIENT_STORAGE, "Insufficient Storage"},
    {HttpResponseStatusCode::LOOP_DETECTED, "Loop Detected"},
    {HttpResponseStatusCode::BANDWIDTH_LIMIT_EXCEEDED,
                "Bandwidth Limit Exceeded"},
    {HttpResponseStatusCode::NOT_EXTENDED, "Not Extended"}
};

QByteArray reasonPhrase(HttpResponseStatusCode statusCode)
{
    return phrases[statusCode];
}

} // namespace Tufao
