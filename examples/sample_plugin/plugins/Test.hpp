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

#ifndef TEST_HPP_
#define TEST_HPP_

#include <Tufao/ClassHandler>

#include <QJsonObject>
#include <QObject>

namespace Tufao {
class HttpServerRequest;
class HttpServerResponse;
}

class Test : public Tufao::ClassHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "tufao.Test/1.0")
	Q_INTERFACES(Tufao::ClassHandler)

public:
	explicit Test();
	virtual ~Test(void);
	void init(void) override;
	void deinit(void) override;
	virtual Tufao::ClassHandlerPluginInfo getPluginInfo() const;

public slots:
    QJsonObject test1(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response);
    void test2(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, int pageNumber);
	void test3(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response);
	void test4(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter);
	void test5(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, QString pageNumber);
	void test6(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, int chapter, int pageNumber);

};

#endif // TEST_HPP_
