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

#ifndef TUFAO_PRIV_HTTPSERVERREQUEST_H
#define TUFAO_PRIV_HTTPSERVERREQUEST_H

#include <boost/http/reader/request.hpp>
#include <boost/beast/http/basic_parser.hpp>

#include "../headers.h"
#include "../httpserverrequest.h"

#include <QtNetwork/QAbstractSocket>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

namespace Tufao {

namespace http = boost::beast::http;
namespace asio = boost::asio;

struct HttpServerRequest::Priv
    : public http::basic_parser<true, HttpServerRequest::Priv>
{
    enum Signal
    {
        READY   = 1,
        DATA    = 1 << 1,
        END     = 1 << 2
    };
    Q_DECLARE_FLAGS(Signals, Signal)

    Priv(Tufao::HttpServerRequest *request, QAbstractSocket &socket) :
        request(request),
        socket(socket),
        useTrailers(false),
        responseOptions(0),
        timeout(0)
    {
        timer.setSingleShot(true);
    }

    void on_request_impl(http::verb, boost::string_view method_str,
                         boost::string_view target, int version,
                         boost::system::error_code&)
    {
        request->clearRequest();
        responseOptions = 0;
        QByteArray method(method_str.data(), method_str.size());
        method = std::move(method);

        QByteArray url(target.data(), target.size());
        url = std::move(url);

        if (version == 10) {
            httpVersion = HttpVersion::HTTP_1_0;
            responseOptions |= HttpServerResponse::HTTP_1_0;
        } else {
            httpVersion = HttpVersion::HTTP_1_1;
            responseOptions |= HttpServerResponse::HTTP_1_1;
        }
    }

    void on_field_impl(http::field, boost::string_view name,
                       boost::string_view value, boost::system::error_code&)
    {
        QByteArray header(value.data(), value.size());
        (useTrailers ? trailers : headers)
            .insert(QByteArray(name.data(), name.size()),
                    std::move(header));
    }

    void on_header_impl(boost::system::error_code&)
    {
        auto it = headers.find("connection");
        bool close_found = false;
        bool keep_alive_found = false;
        for (;it != headers.end();++it) {
            auto value = boost::string_ref(it->data(), it->size());
            boost::http::header_value_any_of(value, [&](boost::string_ref v) {
                    if (iequals(v, "close"))
                        close_found = true;

                    if (iequals(v, "keep-alive"))
                        keep_alive_found = true;

                    if (iequals(v, "upgrade"))
                        is_upgrade = true;

                    return false;
                });
            if (close_found)
                break;
        }
        if (!close_found
            && (httpVersion == HttpVersion::HTTP_1_1 || keep_alive_found)) {
            responseOptions |= HttpServerResponse::KEEP_ALIVE;
        }
        whatEmit = Priv::READY;
        useTrailers = true;
    }

    void on_body_init_impl(const boost::optional<std::uint64_t>&,
                           boost::system::error_code&)
    {}

    std::size_t on_body_impl(boost::string_view s,
                             boost::system::error_code&)
    {
        on_body_or_chunk(s);
        return s.size();
    }

    void on_chunk_header_impl(std::uint64_t, boost::string_view,
                              boost::system::error_code&)
    {}

    std::size_t on_chunk_body_impl(std::uint64_t,
                                   boost::string_view body,
                                   boost::system::error_code&)
    {
        on_body_or_chunk(body);
        return body.size();
    }

    void on_body_or_chunk(boost::string_view value)
    {
        body.append(value.data(), value.size());
        whatEmit |= Priv::DATA;
    }

    void on_finish_impl(boost::system::error_code&)
    {
        useTrailers = false;
        whatEmit |= Priv::END;
        disconnect(&socket, SIGNAL(readyRead()), request, SLOT(onReadyRead()));
    }

    Tufao::HttpServerRequest *request;
    QAbstractSocket &socket;
    QByteArray buffer;
    Signals whatEmit;
    bool is_upgrade;
    bool useTrailers;
    QByteArray body;

    QByteArray method;
    QUrl url;
    Tufao::HttpVersion httpVersion;
    Headers headers;
    Headers trailers;
    Tufao::HttpServerResponse::Options responseOptions;
    QVariant customData;

    int timeout;
    QTimer timer;
};

} // namespace Tufao

#endif // TUFAO_PRIV_HTTPSERVERREQUEST_H
