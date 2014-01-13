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
