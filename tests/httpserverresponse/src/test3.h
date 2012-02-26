#include <Tufao/HttpServerResponse>
#include <QBuffer>
#include <Tufao/Headers>

/*!
  Do a simple HTTP/1.1 stream test.

  \return true if successful
  */
inline void test3()
{
    using namespace Tufao;
    qDebug("Running test3");
    QBuffer b;
    b.open(QIODevice::WriteOnly);

    HttpServerResponse::Options o;
    o |= HttpServerResponse::HTTP_1_1;

    HttpServerResponse r(&b, o);

    r.writeHead(200, "Okay", Headers());
    r.end("42\n");

    Q_ASSERT(!b.isOpen());

    {
        static QByteArray expected("HTTP/1.1 200 Okay\r\n"
                                   "Connection: close\r\n"
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
