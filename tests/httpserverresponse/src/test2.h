#include <Tufao/HttpServerResponse>
#include <QBuffer>

/*!
  Do a HTTP/1.0 stream test with an empty body.

  \return true if successful
  */
inline void test2()
{
    using namespace Tufao;
    qDebug("Running test2");
    QBuffer b;
    b.open(QIODevice::WriteOnly);

    HttpServerResponse::Options o;
    o |= HttpServerResponse::HTTP_1_0;

    HttpServerResponse r(&b, o);

    r.writeHead(HttpServerResponse::OK);
    Q_ASSERT(r.end());
    Q_ASSERT(!b.isOpen());

    {
        static QByteArray expected("HTTP/1.0 200 OK\r\n"
                                   "Content-Length: 0\r\n"
                                   "\r\n");
        Q_ASSERT(b.buffer() == expected);
    }
}
