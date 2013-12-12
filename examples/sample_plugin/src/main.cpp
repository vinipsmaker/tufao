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

	Tufao::ClassHandlerManager classHandler("tufao.Test/1.0");

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
