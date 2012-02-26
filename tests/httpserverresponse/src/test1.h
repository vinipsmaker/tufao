#include <Tufao/HttpServerResponse>
#include <Tufao/Headers>
#include <QBuffer>

/*!
  Do a simple HTTP/1.0 stream test.

  \return true if successful
  */
inline void test1()
{
    using namespace Tufao;
    qDebug("Running test1");
    QBuffer b;
    b.open(QIODevice::WriteOnly);

    HttpServerResponse::Options o;
    o |= HttpServerResponse::HTTP_1_0;

    HttpServerResponse r(&b, o);

    Q_ASSERT(!r.end());
    Q_ASSERT(r.writeHead(HttpServerResponse::OK));
    Q_ASSERT(!r.writeHead(200));
    Q_ASSERT(r.write("This string should be buffered and appear in the final"
                     " response\n"));
    r.headers().insert("Content-Type", "text/plain");
    Q_ASSERT(r.end("42\n"));
    Q_ASSERT(!b.isOpen());

    {
        static QByteArray expected("HTTP/1.0 200 OK\r\n"
                                   "Content-Length: 67\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "\r\n"
                                   "This string should be buffered and appear"
                                   " in the final response\n"
                                   "42\n");
        Q_ASSERT(b.buffer() == expected);
    }
}
