#include <Tufao/HttpServerResponse>
#include <QBuffer>
#include <Tufao/Headers>

/*!
  Do a simple HTTP/1.1 keep-alive empty data stream test.

  Test 2 trailers.

  \return true if successful
  */
inline void test9()
{
    using namespace Tufao;
    qDebug("Running test9");
    QBuffer b;
    b.open(QIODevice::WriteOnly);

    HttpServerResponse::Options o;
    o |= HttpServerResponse::HTTP_1_1;
    o |= HttpServerResponse::KEEP_ALIVE;

    HttpServerResponse r(&b, o);

    r.writeHead(200);
    r.headers().replace("Trailer", "Content-MD5, Content-Type");
    r.write("help");
    {
        Headers trailers;
        trailers.replace("Content-MD5", "45b758a4f518f3ff31363696132f5f5a");
        trailers.replace("Content-Type", "text/plain");
        r.addTrailers(trailers);
    }
    r.end();

    Q_ASSERT(b.isOpen());

    {
        static QByteArray expected("HTTP/1.1 200 OK\r\n"
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
        Q_ASSERT(b.buffer() == expected);
    }
}
