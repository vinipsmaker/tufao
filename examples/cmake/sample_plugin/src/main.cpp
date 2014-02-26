/*
    Copyright (c) 2013 Timothy Reaves treaves@silverfieldstech.com

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

	Tufao::ClassHandlerManager classHandler("tufao.Test/1.0", "/context");

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
