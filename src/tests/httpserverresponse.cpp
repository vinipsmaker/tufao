#include "httpserverresponse.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include <QtCore/QBuffer>
#include "../httpserverresponse.h"

using namespace Tufao;

void HttpServerResponseTest::statusCode_data()
{
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("reasonPhrase");

    QTest::newRow("OK") << 200 << QByteArray{"OK"};
    QTest::newRow("No Content") << 204 << QByteArray{"No Content"};
    QTest::newRow("Moved Permanently")
        << 301 << QByteArray{"Moved Permanently"};
    QTest::newRow("Bad request") << 400 << QByteArray{"Bad Request"};
    QTest::newRow("Not found") << 404 << QByteArray{"Not Found"};
    QTest::newRow("I'm a teapot") << 418 << QByteArray{"I'm a teapot"};
    QTest::newRow("Internal Server Error")
        << 500 << QByteArray{"Internal Server Error"};
}

void HttpServerResponseTest::statusCode()
{
    QFETCH(int, statusCode);
    QFETCH(QByteArray, reasonPhrase);

    HttpResponseStatus status;

    *(reinterpret_cast<int*>(&status)) = statusCode;

    QBuffer buffer;
    HttpServerResponse response{buffer, HttpServerResponse::HTTP_1_0};

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(status));
    QVERIFY(response.end());

    QStringList statusLine
        = QString{buffer.data()}.split("\r\n")[0].split(' ').mid(2);

    QCOMPARE(statusLine.join(' '), QString{reasonPhrase});
}

void HttpServerResponseTest::customReasonPhrases_data()
{
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("reasonPhrase");

    QTest::newRow("199 - I'm Chuck Norris")
        << 199 << QByteArray{"I'm Chuck Norris"};
    QTest::newRow("200 - Each of us bears his own Hell.")
        << 200 << QByteArray{"Each of us bears his own Hell."};
}

void HttpServerResponseTest::customReasonPhrases()
{
    QFETCH(int, statusCode);
    QFETCH(QByteArray, reasonPhrase);

    QBuffer buffer;
    HttpServerResponse response{buffer, HttpServerResponse::HTTP_1_0};

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(statusCode, reasonPhrase));
    QVERIFY(response.end());

    QStringList statusLine
        = QString{buffer.data()}.split("\r\n")[0].split(' ').mid(2);

    QCOMPARE(statusLine.join(' '), QString{reasonPhrase});
}

void HttpServerResponseTest::normalMessages_data()
{
    QTest::addColumn<QByteArray>("body");

    QTest::newRow("Simple body")
        << QByteArray{"The end of the world party\n"};

    QTest::newRow("Another simple body")
        << QByteArray{"Can you hear me major tom?\n"};

    QTest::newRow("Yet another simple body")
        << QByteArray{"Do the evolution!\n"};
}

void HttpServerResponseTest::normalMessages()
{
    QFETCH(QByteArray, body);

    QBuffer buffer;
    HttpServerResponse response{buffer, HttpServerResponse::HTTP_1_0};

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(HttpResponseStatus::OK));
    QVERIFY(response.end(body));

    QVERIFY(buffer.data().endsWith(body));
}

void HttpServerResponseTest::chunkedEntities_data()
{
    QTest::addColumn<QList<QByteArray>>("chunks");
    QTest::addColumn<QByteArray>("body");

    {
        QList<QByteArray> chunks;
        chunks << "Wiki" << "pedia" << " in\r\n\r\nchunks.";
        QTest::newRow("Wikipedia example") << chunks << QByteArray{
            "4\r\n"
            "Wiki\r\n"
            "5\r\n"
            "pedia\r\n"
            "e\r\n"
            " in\r\n"
            "\r\n"
                //"7\r\n"
            "chunks.\r\n"
            "0\r\n"
            "\r\n"
        };
    }
}

void HttpServerResponseTest::chunkedEntities()
{
    QFETCH(QList<QByteArray>, chunks);
    QFETCH(QByteArray, body);

    QBuffer buffer;
    HttpServerResponse response{buffer, HttpServerResponse::HTTP_1_1};

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(HttpResponseStatus::OK));

    for (int i = 0;i != chunks.size() - 1;++i)
        QVERIFY(response.write(chunks[i]));

    QVERIFY(response.end(chunks[chunks.size() - 1]));

    QVERIFY(buffer.data().endsWith(body));
}

void HttpServerResponseTest::httpContinue()
{
    QBuffer buffer;
    HttpServerResponse response{buffer, HttpServerResponse::HTTP_1_1};

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeContinue());

    QCOMPARE(buffer.data(), QByteArray{"HTTP/1.1 100 Continue\r\n\r\n"});
}

void HttpServerResponseTest::options()
{
    QBuffer buffer;
    {
        buffer.open(QIODevice::WriteOnly);
        HttpServerResponse response{buffer, HttpServerResponse::HTTP_1_0};
        response.writeHead(HttpResponseStatus::OK);
        response.end();
        QVERIFY(!buffer.isOpen());
        QVERIFY(buffer.data().startsWith("HTTP/1.0 200 OK\r\n"));
    }
    {
        buffer.open(QIODevice::WriteOnly);
        HttpServerResponse response{buffer, HttpServerResponse::HTTP_1_1};
        response.writeHead(HttpResponseStatus::OK);
        response.end();
        QVERIFY(!buffer.isOpen());
        QVERIFY(buffer.data().startsWith("HTTP/1.1 200 OK\r\n"));
    }
    {
        buffer.open(QIODevice::WriteOnly);
        HttpServerResponse::Options options;
        options |= HttpServerResponse::HTTP_1_1;
        options |= HttpServerResponse::KEEP_ALIVE;
        HttpServerResponse response{buffer, options};
        response.writeHead(HttpResponseStatus::OK);
        response.end();
        QVERIFY(buffer.isOpen());
        QVERIFY(buffer.data().startsWith("HTTP/1.1 200 OK\r\n"));
    }
}

void HttpServerResponseTest::invalid()
{
    QBuffer buffer;

    {
        HttpServerResponse response(buffer, HttpServerResponse::HTTP_1_1);

        QVERIFY(!response.write("test"));
        QVERIFY(!response.addTrailer("key", "value"));
        QVERIFY(!response.end());

        QVERIFY(response.writeContinue());
        QVERIFY(response.writeHead(HttpResponseStatus::OK));

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpResponseStatus::OK));
        QVERIFY(!response.addTrailer("key", "value"));

        QVERIFY(response.write("test"));

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpResponseStatus::OK));

        QVERIFY(response.addTrailer("key", "value"));

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpResponseStatus::OK));

        QVERIFY(response.end());

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpResponseStatus::OK));
        QVERIFY(!response.addTrailer("key", "value"));
    }

    {
        HttpServerResponse response(buffer, HttpServerResponse::HTTP_1_0);

        QVERIFY(!response.writeContinue());

        QVERIFY(response.writeHead(HttpResponseStatus::OK));
        QVERIFY(response.write("test"));

        QVERIFY(!response.addTrailer("key", "value"));

        QVERIFY(response.end());
    }
}

QTEST_APPLESS_MAIN(HttpServerResponseTest)
