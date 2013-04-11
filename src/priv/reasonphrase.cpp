/*  This file is part of the Tufão project
    Copyright (C) 2011-2013 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

std::map<HttpResponseStatus, QByteArray> phrases {
    // 1xx Informational
    {HttpResponseStatus::CONTINUE, "Continue"},
    {HttpResponseStatus::SWITCHING_PROTOCOLS, "Switching Protocols"},
    {HttpResponseStatus::PROCESSING, "Processing"},
    {HttpResponseStatus::CHECKPOINT, "Checkpoint"},

    // 2xx Successful
    {HttpResponseStatus::OK, "OK"},
    {HttpResponseStatus::CREATED, "Created"},
    {HttpResponseStatus::ACCEPTED, "Accepted"},
    {HttpResponseStatus::NON_AUTHORITATIVE_INFORMATION,
                "Non-Authoritative Information"},
    {HttpResponseStatus::NO_CONTENT, "No Content"},
    {HttpResponseStatus::RESET_CONTENT, "Reset Content"},
    {HttpResponseStatus::PARTIAL_CONTENT, "Partial Content"},
    {HttpResponseStatus::MULTI_STATUS, "Multi-Status"},
    {HttpResponseStatus::ALREADY_REPORTED, "Already Reported"},
    {HttpResponseStatus::IM_USED, "IM Used"},

    // 3xx Redirection
    {HttpResponseStatus::MULTIPLE_CHOICES, "Multiple Choices"},
    {HttpResponseStatus::MOVED_PERMANENTLY, "Moved Permanently"},
    {HttpResponseStatus::FOUND, "Found"},
    {HttpResponseStatus::SEE_OTHER, "See Other"},
    {HttpResponseStatus::NOT_MODIFIED, "Not Modified"},
    {HttpResponseStatus::USE_PROXY, "Use Proxy"},
    {HttpResponseStatus::SWITCH_PROXY, "Switch Proxy"},
    {HttpResponseStatus::TEMPORARY_REDIRECT, "Temporary Redirect"},
    {HttpResponseStatus::RESUME_INCOMPLETE, "Resume Incomplete"},

    // 4xx Client Error
    {HttpResponseStatus::BAD_REQUEST, "Bad Request"},
    {HttpResponseStatus::UNAUTHORIZED, "Unauthorized"},
    {HttpResponseStatus::PAYMENT_REQUIRED, "Payment Required"},
    {HttpResponseStatus::FORBIDDEN, "Forbidden"},
    {HttpResponseStatus::NOT_FOUND, "Not Found"},
    {HttpResponseStatus::METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {HttpResponseStatus::NOT_ACCEPTABLE, "Not Acceptable"},
    {HttpResponseStatus::PROXY_AUTHENTICATION_REQUIRED,
                "Proxy Authentication Required"},
    {HttpResponseStatus::REQUEST_TIMEOUT, "Request Timeout"},
    {HttpResponseStatus::CONFLICT, "Conflict"},
    {HttpResponseStatus::GONE, "Gone"},
    {HttpResponseStatus::LENGTH_REQUIRED, "Length Required"},
    {HttpResponseStatus::PRECONDITION_FAILED, "Precondition Failed"},
    {HttpResponseStatus::REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large"},
    {HttpResponseStatus::REQUEST_URI_TOO_LONG, "Request-URI Too Long"},
    {HttpResponseStatus::UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"},
    {HttpResponseStatus::REQUESTED_RANGE_NOT_SATISFIABLE,
                "Requested Range Not Satisfiable"},
    {HttpResponseStatus::EXPECTATION_FAILED, "Expectation Failed"},
    {HttpResponseStatus::I_AM_A_TEAPOT, "I'm a teapot"},
    {HttpResponseStatus::UNPROCESSABLE_ENTITY, "Unprocessable Entity"},
    {HttpResponseStatus::LOCKED, "Locked"},
    {HttpResponseStatus::FAILED_DEPENDENCY, "Failed Dependency"},
    {HttpResponseStatus::UNORDERED_COLLECTION, "Unordered Collection"},
    {HttpResponseStatus::UPGRADE_REQUIRED, "Upgrade Required"},
    {HttpResponseStatus::PRECONDITION_REQUIRED, "Precondition Required"},
    {HttpResponseStatus::TOO_MANY_REQUESTS, "Too Many Requests"},
    {HttpResponseStatus::REQUEST_HEADER_FIELDS_TOO_LARGE,
                "Request Header Fields Too Large"},
    {HttpResponseStatus::NO_RESPONSE, "No Response"},
    {HttpResponseStatus::RETRY_WITH, "Retry With"},
    {HttpResponseStatus::CLIENT_CLOSED_REQUEST, "Client Closed Request"},

    // 5xx Internal Server Error
    {HttpResponseStatus::INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {HttpResponseStatus::NOT_IMPLEMENTED, "Not Implemented"},
    {HttpResponseStatus::BAD_GATEWAY, "Bad Gateway"},
    {HttpResponseStatus::SERVICE_UNAVAILABLE, "Service Unavailable"},
    {HttpResponseStatus::GATEWAY_TIMEOUT, "Gateway Timeout"},
    {HttpResponseStatus::HTTP_VERSION_NOT_SUPPORTED,
                "HTTP Version Not Supported"},
    {HttpResponseStatus::VARIANT_ALSO_NEGOTIATES, "Variant Also Negotiates"},
    {HttpResponseStatus::INSUFFICIENT_STORAGE, "Insufficient Storage"},
    {HttpResponseStatus::LOOP_DETECTED, "Loop Detected"},
    {HttpResponseStatus::BANDWIDTH_LIMIT_EXCEEDED, "Bandwidth Limit Exceeded"},
    {HttpResponseStatus::NOT_EXTENDED, "Not Extended"}
};

QByteArray reasonPhrase(HttpResponseStatus statusCode)
{
    return phrases[statusCode];
}

} // namespace Tufao
