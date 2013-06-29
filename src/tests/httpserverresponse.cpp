#include "httpserverresponse.h"
#include <QtTest/QTest>
#include <QtCore/QLocale>
#include <QtCore/QBuffer>
#include "../httpserverresponse.h"
#include "../headers.h"

using namespace Tufao;

typedef QList< QPair<QByteArray, QByteArray> > PredictableHeaders;
Q_DECLARE_METATYPE(PredictableHeaders)

Q_DECLARE_METATYPE(Headers)

void HttpServerResponseTest::statusCode_data()
{
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("reasonPhrase");

    QTest::newRow("OK") << 200 << QByteArray("OK");
    QTest::newRow("No Content") << 204 << QByteArray("No Content");
    QTest::newRow("Moved Permanently")
        << 301 << QByteArray("Moved Permanently");
    QTest::newRow("Bad request") << 400 << QByteArray("Bad Request");
    QTest::newRow("Not found") << 404 << QByteArray("Not Found");
    QTest::newRow("I'm a teapot") << 418 << QByteArray("I'm a teapot");
    QTest::newRow("Internal Server Error")
        << 500 << QByteArray("Internal Server Error");
}

void HttpServerResponseTest::statusCode()
{
    QFETCH(int, statusCode);
    QFETCH(QByteArray, reasonPhrase);

    HttpServerResponse::StatusCode status;

    *(reinterpret_cast<int*>(&status)) = statusCode;

    QBuffer buffer;
    HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_0);

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(status));
    QVERIFY(response.end());

    QStringList statusLine
        = QString(buffer.data()).split("\r\n")[0].split(' ').mid(2);

    QCOMPARE(statusLine.join(" "), QString(reasonPhrase));
}

void HttpServerResponseTest::customReasonPhrases_data()
{
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("reasonPhrase");

    QTest::newRow("199 - I'm Chuck Norris")
        << 199 << QByteArray("I'm Chuck Norris");
    QTest::newRow("200 - Each of us bears his own Hell.")
        << 200 << QByteArray("Each of us bears his own Hell.");
}

void HttpServerResponseTest::customReasonPhrases()
{
    QFETCH(int, statusCode);
    QFETCH(QByteArray, reasonPhrase);

    QBuffer buffer;
    HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_0);

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(statusCode, reasonPhrase));
    QVERIFY(response.end());

    QStringList statusLine
        = QString(buffer.data()).split("\r\n")[0].split(' ').mid(2);

    QCOMPARE(statusLine.join(" "), QString(reasonPhrase));
}

void HttpServerResponseTest::normalMessages_data()
{
    QTest::addColumn<QByteArray>("body");

    QTest::newRow("Simple body")
        << QByteArray("The end of the world party\n");

    QTest::newRow("Another simple body")
        << QByteArray("Can you hear me major tom?\n");

    QTest::newRow("Yet another simple body")
        << QByteArray("Do the evolution!\n");
}

void HttpServerResponseTest::normalMessages()
{
    QFETCH(QByteArray, body);

    QBuffer buffer;
    HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_0);

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(HttpServerResponse::OK));
    QVERIFY(response.end(body));

    QVERIFY(buffer.data().endsWith(body));
}

void HttpServerResponseTest::generalMessages_data()
{
    QTest::addColumn<int>("options");
    QTest::addColumn<int>("head");
    QTest::addColumn<QByteArray>("reasonPhrase");
    QTest::addColumn<Headers>("headers");
    QTest::addColumn<QStringList>("chunks");
    QTest::addColumn<PredictableHeaders>("trailers");
    QTest::addColumn<QByteArray>("end");
    QTest::addColumn<QByteArray>("expected");

    {
        // Do a simple HTTP/1.0 stream test.

        Headers headers;
        headers.insert("Content-Type", "text/plain");

        QStringList chunks;
        chunks << ("This string should be buffered and appear in the final"
                   " response\n");

        QTest::newRow("test1")
            << int(HttpServerResponse::HTTP_1_0) << 200 << QByteArray()
            << headers << chunks << PredictableHeaders() << QByteArray("42\n")
            << QByteArray("HTTP/1.0 200 OK\r\n"
                          "Content-Length: 67\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "This string should be buffered and appear"
                          " in the final response\n"
                          "42\n");
    }
    {
        // Do a HTTP/1.0 stream test with an empty body.

        QTest::newRow("test2")
            << int(HttpServerResponse::HTTP_1_0) << 200 << QByteArray()
            << Headers() << QStringList() << PredictableHeaders() << QByteArray()
            << QByteArray("HTTP/1.0 200 OK\r\n"
                          "Content-Length: 0\r\n"
                          "\r\n");
    }
    {
        // Do a simple HTTP/1.1 stream test.

        QTest::newRow("test3")
            << int(HttpServerResponse::HTTP_1_1) << 200 << QByteArray("Okay")
            << Headers() << QStringList() << PredictableHeaders()
            << QByteArray("42\n")
            << QByteArray("HTTP/1.1 200 Okay\r\n"
                          "Connection: close\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "3\r\n"
                          "42\n"
                          "\r\n"
                          "0\r\n"
                          "\r\n");
    }
    {
        // Do a simple HTTP/1.1 keep-alive stream test.

        QTest::newRow("test4")
            << (HttpServerResponse::HTTP_1_1 | HttpServerResponse::KEEP_ALIVE)
            << 200 << QByteArray() << Headers() << QStringList()
            << PredictableHeaders() << QByteArray("42\n")
            << QByteArray("HTTP/1.1 200 OK\r\n"
                          "Connection: keep-alive\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "3\r\n"
                          "42\n"
                          "\r\n"
                          "0\r\n"
                          "\r\n");
    }
    {
        // Do a simple HTTP/1.1 keep-alive stream test.
        // Test HttpServerResponse::write

        QStringList chunks;
        chunks << "The number is ";

        QTest::newRow("test5")
            << (HttpServerResponse::HTTP_1_1 | HttpServerResponse::KEEP_ALIVE)
            << 200 << QByteArray() << Headers() << chunks
            << PredictableHeaders() << QByteArray("42\n")
            << QByteArray("HTTP/1.1 200 OK\r\n"
                          "Connection: keep-alive\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "e\r\n"
                          "The number is \r\n"
                          "3\r\n"
                          "42\n"
                          "\r\n"
                          "0\r\n"
                          "\r\n");
    }
    {
        // Do a simple HTTP/1.1 keep-alive stream test.
        // Test HttpServerResponse::write and a empty HttpServerResponse::end

        QStringList chunks;
        chunks << "The number is " << "42\n";

        QTest::newRow("test6")
            << (HttpServerResponse::HTTP_1_1 | HttpServerResponse::KEEP_ALIVE)
            << 200 << QByteArray() << Headers() << chunks
            << PredictableHeaders() << QByteArray()
            << QByteArray("HTTP/1.1 200 OK\r\n"
                          "Connection: keep-alive\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "e\r\n"
                          "The number is \r\n"
                          "3\r\n"
                          "42\n"
                          "\r\n"
                          "0\r\n"
                          "\r\n");
    }
    {
        // Do a simple HTTP/1.1 keep-alive empty data stream test.

        QTest::newRow("test7")
            << (HttpServerResponse::HTTP_1_1 | HttpServerResponse::KEEP_ALIVE)
            << 200 << QByteArray() << Headers() << QStringList()
            << PredictableHeaders() << QByteArray()
            << QByteArray("HTTP/1.1 200 OK\r\n"
                          "Connection: keep-alive\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "0\r\n"
                          "\r\n");
    }
    {
        //Do a simple HTTP/1.1 keep-alive empty data stream test.
        // Test a trailer.

        Headers headers;
        headers.replace("Trailer", "Content-MD5");

        QStringList chunks;
        chunks << "help";

        PredictableHeaders trailers;
        trailers << QPair<QByteArray, QByteArray>("Content-MD5",
                                                  "45b758a4f518f3ff"
                                                  "31363696132f5f5a");

        QTest::newRow("test8")
            << (HttpServerResponse::HTTP_1_1 | HttpServerResponse::KEEP_ALIVE)
            << 200 << QByteArray() << headers << chunks << trailers
            << QByteArray()
            << QByteArray("HTTP/1.1 200 OK\r\n"
                          "Connection: keep-alive\r\n"
                          "Trailer: Content-MD5\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "4\r\n"
                          "help\r\n"
                          "0\r\n"
                          "Content-MD5: 45b758a4f518f3ff31363696132f5f5a\r\n"
                          "\r\n");
    }
    {
        // Do a simple HTTP/1.1 keep-alive empty data stream test
        // Test 2 trailers.

        Headers headers;
        headers.replace("Trailer", "Content-MD5, Content-Type");

        QStringList chunks;
        chunks << "help";

        PredictableHeaders trailers;
        trailers
            << QPair<QByteArray, QByteArray>("Content-MD5",
                                             "45b758a4f518f3ff31363696132f5f5a")
            << QPair<QByteArray, QByteArray>("Content-Type", "text/plain");

        QTest::newRow("test9")
            << (HttpServerResponse::HTTP_1_1 | HttpServerResponse::KEEP_ALIVE)
            << 200 << QByteArray() << headers << chunks << trailers
            << QByteArray()
            << QByteArray("HTTP/1.1 200 OK\r\n"
                          "Connection: keep-alive\r\n"
                          "Trailer: Content-MD5, Content-Type\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "4\r\n"
                          "help\r\n"
                          "0\r\n"
                          "Content-MD5: 45b758a4f518f3ff31363696132f5f5a\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n");
    }
}

void HttpServerResponseTest::generalMessages()
{
    QFETCH(int, options);
    QFETCH(int, head);
    QFETCH(QByteArray, reasonPhrase);
    QFETCH(Headers, headers);
    QFETCH(QStringList, chunks);
    QFETCH(PredictableHeaders, trailers);
    QFETCH(QByteArray, end);
    QFETCH(QByteArray, expected);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    HttpServerResponse::Option o;
    *(reinterpret_cast<int*>(&o)) = options;

    HttpServerResponse response(&buffer, o);

    if (reasonPhrase.size())
        response.writeHead(head, reasonPhrase);
    else
        response.writeHead(head);

    response.headers() = headers;

    for (int i = 0;i != chunks.size();++i)
        response.write(chunks[i].toUtf8());

    for (int i = 0;i != trailers.size();++i)
        response.addTrailer(trailers[i].first, trailers[i].second);

    response.end(end);

    QCOMPARE(buffer.isOpen(), bool(o & HttpServerResponse::KEEP_ALIVE));

    QCOMPARE(buffer.buffer(), expected);
}

void HttpServerResponseTest::http10Buffering()
{
    QBuffer b;
    b.open(QIODevice::WriteOnly);

    HttpServerResponse::Options o;
    o |= HttpServerResponse::HTTP_1_0;

    HttpServerResponse r(&b, o);

    QVERIFY(!r.end());
    QVERIFY(r.writeHead(HttpServerResponse::OK));
    QVERIFY(!r.writeHead(200));
    QVERIFY(r.write("This string should be buffered and appear in the final"
                    " response\n"));
    r.headers().insert("Content-Type", "text/plain");
    QVERIFY(r.end("42\n"));
    QVERIFY(!b.isOpen());

    QCOMPARE(b.buffer(),
             QByteArray("HTTP/1.0 200 OK\r\n"
                        "Content-Length: 67\r\n"
                        "Content-Type: text/plain\r\n"
                        "\r\n"
                        "This string should be buffered and appear"
                        " in the final response\n"
                        "42\n"));
}

void HttpServerResponseTest::chunkedEntities_data()
{
    QTest::addColumn<QStringList>("chunks");
    QTest::addColumn<QByteArray>("body");

    {
        QStringList chunks;
        chunks << "Wiki" << "pedia" << " in\r\n\r\nchunks.";
        QTest::newRow("Wikipedia example") << chunks <<
            QByteArray("4\r\n"
                       "Wiki\r\n"
                       "5\r\n"
                       "pedia\r\n"
                       "e\r\n"
                       " in\r\n"
                       "\r\n"
                       //"7\r\n"
                       "chunks.\r\n"
                       "0\r\n"
                       "\r\n");
    }
}

void HttpServerResponseTest::chunkedEntities()
{
    QFETCH(QStringList, chunks);
    QFETCH(QByteArray, body);

    QBuffer buffer;
    HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_1);

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeHead(HttpServerResponse::OK));

    for (int i = 0;i != chunks.size() - 1;++i)
        QVERIFY(response.write(chunks[i].toUtf8()));

    QVERIFY(response.end(chunks[chunks.size() - 1].toUtf8()));

    QVERIFY(buffer.data().endsWith(body));
}

void HttpServerResponseTest::httpContinue()
{
    QBuffer buffer;
    HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_1);

    buffer.open(QIODevice::WriteOnly);
    QVERIFY(response.writeContinue());

    QCOMPARE(buffer.data(), QByteArray("HTTP/1.1 100 Continue\r\n\r\n"));
}

void HttpServerResponseTest::options()
{
    QBuffer buffer;
    {
        buffer.open(QIODevice::WriteOnly);
        HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_0);
        response.writeHead(HttpServerResponse::OK);
        response.end();
        QVERIFY(!buffer.isOpen());
        QVERIFY(buffer.data().startsWith("HTTP/1.0 200 OK\r\n"));
    }
    {
        buffer.open(QIODevice::WriteOnly);
        HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_1);
        response.writeHead(HttpServerResponse::OK);
        response.end();
        QVERIFY(!buffer.isOpen());
        QVERIFY(buffer.data().startsWith("HTTP/1.1 200 OK\r\n"));
    }
    {
        buffer.open(QIODevice::WriteOnly);
        HttpServerResponse::Options options;
        options |= HttpServerResponse::HTTP_1_1;
        options |= HttpServerResponse::KEEP_ALIVE;
        HttpServerResponse response(&buffer, options);
        response.writeHead(HttpServerResponse::OK);
        response.end();
        QVERIFY(buffer.isOpen());
        QVERIFY(buffer.data().startsWith("HTTP/1.1 200 OK\r\n"));
    }
}

void HttpServerResponseTest::invalid()
{
    QBuffer buffer;

    {
        HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_1);

        QVERIFY(!response.write("test"));
        QVERIFY(!response.addTrailer("key", "value"));
        QVERIFY(!response.end());

        QVERIFY(response.writeContinue());
        QVERIFY(response.writeHead(HttpServerResponse::OK));

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpServerResponse::OK));
        QVERIFY(!response.addTrailer("key", "value"));

        QVERIFY(response.write("test"));

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpServerResponse::OK));

        QVERIFY(response.addTrailer("key", "value"));

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpServerResponse::OK));

        QVERIFY(response.end());

        QVERIFY(!response.writeContinue());
        QVERIFY(!response.writeHead(HttpServerResponse::OK));
        QVERIFY(!response.addTrailer("key", "value"));
    }

    {
        HttpServerResponse response(&buffer, HttpServerResponse::HTTP_1_0);

        QVERIFY(!response.writeContinue());

        QVERIFY(response.writeHead(HttpServerResponse::OK));
        QVERIFY(response.write("test"));

        QVERIFY(!response.addTrailer("key", "value"));

        QVERIFY(response.end());
    }
}

QTEST_APPLESS_MAIN(HttpServerResponseTest)
