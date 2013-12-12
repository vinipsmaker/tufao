#ifndef TEST_HPP_
#define TEST_HPP_

#include <Tufao/ClassHandler>

#include <QJsonDocument>
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
	QJsonDocument test1(QString chapter, int pageNumber);
	void test2(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, int pageNumber);
	void test3(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response);
	void test4(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter);
	void test5(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, QString chapter, QString pageNumber);
	void test6(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response, int chapter, int pageNumber);

};

#endif // TEST_HPP_
