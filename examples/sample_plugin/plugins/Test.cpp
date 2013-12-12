#include "Test.hpp"

#include <Tufao/Headers>
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpServerResponse>

#include <QDebug>

/* ****************************************************************************************************************** */
#pragma mark -
#pragma mark Object lifecycle
/* ****************************************************************************************************************** */
Test::Test()
{
	setObjectName("test");
}

Test::~Test()
{

}

/* ****************************************************************************************************************** */
#pragma mark -
#pragma mark Slots
/* ****************************************************************************************************************** */
QJsonDocument Test::test1(QString chapter, int pageNumber)
{
	return QJsonDocument();
}

void Test::test2(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, int pageNumber)
{
	Q_UNUSED(request)
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test2 called with chapter value of " + chapter +
					 " and pageNumber value of " + pageNumber +
					 ".</p></body></html>");
	response.end(html.toUtf8());
	qDebug() << QString("==> %1,%2").arg(chapter).arg(pageNumber);
}

void Test::test3(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response)
{
	Q_UNUSED(request)
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	response << "<html><head><title>Plugin Test</title></head><body>"
				<< "<p>test3 called.</p></body></html>";
	response.end();
}

void Test::test4(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter)
{
	Q_UNUSED(request)
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test4 called with chapter value of " + chapter +
					 ".</p></body></html>");
	response.end(html.toUtf8());

}

void Test::test5(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, QString pageNumber)
{
	Q_UNUSED(request)
	qDebug() << QString("==> %1,%2").arg(chapter).arg(pageNumber);
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test5 called with chapter value of " + chapter +
					 " and pageNumber value of " + pageNumber +
					 ".</p></body></html>");
	response.end(html.toUtf8());
}

void Test::test6(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, int chapter, int pageNumber)
{
	Q_UNUSED(request)
	qDebug() << QString("==> %1,%2").arg(chapter).arg(pageNumber);
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test6 called with chapter value of " + QString(chapter) +
					 " and pageNumber value of " + QString(pageNumber) +
					 ".</p></body></html>");
	response.end(html.toUtf8());
}

/* ****************************************************************************************************************** */
#pragma mark -
#pragma mark Override ClassHandlerInterface Methods
/* ****************************************************************************************************************** */
Tufao::ClassHandlerPluginInfo Test::getPluginInfo() const
{
//	// Allow to load the resources when used as a static plugin
//	Q_INIT_RESOURCE(Test);

	Tufao::ClassHandlerPluginInfo info;
	info.id = "Test";
	info.displayedName = ("Test class handler");
	info.authors = "Timothy Reaves";
	info.contact = "treaves@silverfieldstech.com";
	info.description = ("A test of the ClassHandler plugin mechinism.");
	return info;
}


void Test::init(void)
{

}

void Test::deinit(void)
{

}

