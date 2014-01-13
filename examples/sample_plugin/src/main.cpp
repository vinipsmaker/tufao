/*
    Copyright (c) 2013 Timothy Reaves treaves@silverfieldstech.com

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

#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QRegularExpression>

#include <Tufao/ClassHandlerManager>
#include <Tufao/HttpServer>
#include <Tufao/HttpServerRequestRouter>
#include <Tufao/HttpFileServer>
#include <Tufao/UrlRewriterHandler>
#include <Tufao/NotFoundHandler>

#include "main.hpp"


const char * NS_SERVER_SETTINGS = "eml.ServerSettings";


int main(int argc, char *argv[])
{
	QCoreApplication qtApplication(argc, argv);
	Tufao::HttpServer server;

	Tufao::ClassHandlerManager classHandler("tufao.Test/1.0", "context");

	Tufao::HttpServerRequestRouter router{
		{QRegularExpression{"^/$"}, Tufao::UrlRewriterHandler::handler(QUrl("/index.html"))},
		{QRegularExpression{""}, Tufao::HttpFileServer::handler("static")},
		{QRegularExpression{""}, classHandler},
		{QRegularExpression{""}, Tufao::NotFoundHandler::handler()}
	};


	QObject::connect(&server, &Tufao::HttpServer::requestReady, &router, &Tufao::HttpServerRequestRouter::handleRequest);

	server.listen(QHostAddress::Any, 8000);

	return qtApplication.exec();
}
