#include <Tufao/HttpServerResponse>
#include <QBuffer>

/*!
  Do a simple HTTP/1.1 keep-alive stream test.

  \return true if successful
  */
inline void test4()
{
    using namespace Tufao;
    qDebug("Running test4");
    QBuffer b;
    b.open(QIODevice::WriteOnly);

    HttpServerResponse::Options o;
    o |= HttpServerResponse::HTTP_1_1;
    o |= HttpServerResponse::KEEP_ALIVE;

    HttpServerResponse r(&b, o);

    r.writeHead(200);
    r.end("42\n");

    Q_ASSERT(b.isOpen());

    {
        static QByteArray expected("HTTP/1.1 200 OK\r\n"
                                   "Connection: keep-alive\r\n"
                                   "Transfer-Encoding: chunked\r\n"
                                   "\r\n"
                                   "3\r\n"
                                   "42\n"
                                   "\r\n"
                                   "0\r\n"
                                   "\r\n");
        Q_ASSERT(b.buffer() == expected);
    }
}
