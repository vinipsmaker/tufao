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

#include "Test.hpp"

#include <Tufao/Headers>
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpServerResponse>

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrl>

/* ************************************************************************** */
/* Object lifecycle                                                           */
/* ************************************************************************** */
Test::Test()
{
	setObjectName("test");
}

Test::~Test()
{

}

/* ************************************************************************** */
/* Slots                                                                      */
/* ************************************************************************** */
QJsonObject Test::test1(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response)
{
    Q_UNUSED(request)
    Q_UNUSED(response)
    QJsonObject responsePayload;
    responsePayload["FirstName"] = QString("John");
    responsePayload["LastName"] = QString("Doe");
    responsePayload["Age"] = 43;
    QJsonObject address;
    address["Street"] = QString("Downing Street 10");
    address["City"] = QString("London");
    address["Country"] = QString("Great Britain");
    responsePayload["Address"] = address;
    QJsonArray numbers;
    numbers.append(QString("+44 1234567"));
    numbers.append(QString("+44 2345678"));
    responsePayload["Phone numbers"] = numbers;

    QJsonObject jsonResponse;
    jsonResponse[HttpResponseStatusKey] = int(Tufao::HttpResponseStatus::OK);
    jsonResponse[JsonResponseKey] = responsePayload;

    return jsonResponse;

}

void Test::test2(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, int pageNumber)
{
    qDebug() << "I don't know if my view from the environment is the same,"
        " because namespaces transparently override what I can see.";
    qDebug() << "I think I'm handling this url: " << request.url();

	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test2 called with chapter value of " + chapter +
					 " and pageNumber value of " + QVariant(pageNumber).toString() +
					 ".</p></body></html>");
	response.end(html.toUtf8());
	qDebug() << QString("==> %1,%2").arg(chapter).arg(pageNumber);
}

void Test::test3(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response)
{
    qDebug() << "I don't know if my view from the environment is the same,"
        " because namespaces transparently override what I can see.";
    qDebug() << "I think I'm handling this url: " << request.url();

	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	response << "<html><head><title>Plugin Test</title></head><body>"
				<< "<p>test3 called.</p></body></html>";
	response.end();
}

void Test::test4(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter)
{
    qDebug() << "I don't know if my view from the environment is the same,"
        " because namespaces transparently override what I can see.";
    qDebug() << "I think I'm handling this url: " << request.url();

	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test4 called with chapter value of " + chapter +
					 ".</p></body></html>");
	response.end(html.toUtf8());

}

void Test::test5(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, QString pageNumber)
{
    qDebug() << "I don't know if my view from the environment is the same,"
        " because namespaces transparently override what I can see.";
    qDebug() << "I think I'm handling this url: " << request.url();

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
    qDebug() << "I don't know if my view from the environment is the same,"
        " because namespaces transparently override what I can see.";
    qDebug() << "I think I'm handling this url: " << request.url();

	qDebug() << QString("==> %1,%2").arg(chapter).arg(pageNumber);
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test6 called with chapter value of " + QVariant(chapter).toString()+
					 " and pageNumber value of " + QVariant(pageNumber).toString() +
					 ".</p></body></html>");
	response.end(html.toUtf8());
}

/* ************************************************************************** */
/* Override ClassHandlerInterface Methods                                     */
/* ************************************************************************** */
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

