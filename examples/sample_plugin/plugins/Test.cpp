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

#include "Test.hpp"

#include <Tufao/Headers>
#include <Tufao/HttpServerRequest>
#include <Tufao/HttpServerResponse>

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>

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
    jsonResponse[Tufao::HttpResponseStatusKey] = int(Tufao::HttpResponseStatus::OK);
    jsonResponse[Tufao::JsonResponseKey] = responsePayload;

    return jsonResponse;

}

void Test::test2(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, int pageNumber)
{
	if(!request.context().isEmpty()){
		qDebug() << "The context for this plugin is: " << request.context();
	}
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
	if(!request.context().isEmpty()){
		qDebug() << "The context for this plugin is: " << request.context();
	}
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	response << "<html><head><title>Plugin Test</title></head><body>"
				<< "<p>test3 called.</p></body></html>";
	response.end();
}

void Test::test4(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter)
{
	if(!request.context().isEmpty()){
		qDebug() << "The context for this plugin is: " << request.context();
	}
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test4 called with chapter value of " + chapter +
					 ".</p></body></html>");
	response.end(html.toUtf8());

}

void Test::test5(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, QString pageNumber)
{
	if(!request.context().isEmpty()){
		qDebug() << "The context for this plugin is: " << request.context();
	}
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
	if(!request.context().isEmpty()){
		qDebug() << "The context for this plugin is: " << request.context();
	}
	qDebug() << QString("==> %1,%2").arg(chapter).arg(pageNumber);
	response.writeHead(Tufao::HttpResponseStatus::OK);
	response.headers().replace("Content-Type", "text/html; charset=utf-8");
	QString html("<html><head><title>Plugin Test</title></head><body>"
					 "<p>test6 called with chapter value of " + QVariant(chapter).toString()+
					 " and pageNumber value of " + QVariant(pageNumber).toString() +
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

