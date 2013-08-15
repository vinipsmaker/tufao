/*
  Copyright (c) 2012 Vinícius dos Santos Oliveira

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

#ifndef TUFAO_THREADEDHTTPREQUESTDISTPATCHER_H
#define TUFAO_THREADEDHTTPREQUESTDISTPATCHER_H

#include <functional>
#include <initializer_list>

#include <QtCore/QRegularExpression>
#include <QtCore/QObject>
#include <QtCore/QDebug>

#include "tufao_global.h"
#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

/*!
  This class provides a robust and high performance Threaded HTTP request dispatcher.

  It takes a incoming request and forwards it to one of its internal worker threads.
  The size of the threadpool can be customized. The handleRequest function will
  ALWAYS return true, because there is no way the dispatcher can know if the request
  was handled properly inside one of the threads.

  The dispatcher supports asynchronous programming and will be attached to a request
  until finished() was emitted from the response object, or the connection is closed.

  \warning
  Always call end() on your response object or the thread will be stuck forever with
  the request and the dispatcher will run out of threads.


  The code below provides an example usage:

  \include threadedapplicationdefaultmain.cpp

  \warning
  Do NEVER delete a Factory as long as the threads are running
  If you need to delete it, you have to unregister it from the Router and call reload()
  Only after that call its safe to delete a Factory object

  \since
  0.3
  */
class TUFAO_EXPORT ThreadedHttpRequestDispatcher: public QObject,
                                            public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
     It's a simple typedef for the type of handler accepted by the
     HttpServerRequestRouter.

     \since
     1.0
     */
    typedef std::function<AbstractHttpServerRequestHandler* ()>
    Factory;

    /*!
      Constructs a HttpServerRequestRouter object.
      */
    explicit ThreadedHttpRequestDispatcher(Factory threadInitializer, QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~ThreadedHttpRequestDispatcher();

    /*!
      \brief setThreadPoolSize
      Sets the number of threads that handle incoming requests
      \param size
      \note changing this after the eventloop was started requires to call restart()
     */
    void setThreadPoolSize (const unsigned int size);

    /*!
     * \brief restart
     *Will unload all threads and restart them
     */
    void restart ();

// QObject interface
public:
    virtual bool event(QEvent *e);

public slots:
    /*!
      It will route the request to the right handler thread.

      \return
      Returns always true if there is a thread to handle the request.
      However the thread might not be handled inside the Thread if there is no
      correct request handler chain set. If a thread can not handle a request it will
      delete it.

      \since
      1.0
      */
    bool handleRequest(Tufao::HttpServerRequest &request,
                       Tufao::HttpServerResponse &response) override;

private slots:
    void initializeThreads();

private:
    struct Priv;
    Priv *priv;

};

/**
 * @brief tDebug
 * @return
 * Calls qDebug() but prefixes the string with the threadId
 */
TUFAO_EXPORT QDebug tDebug ();

} // namespace Tufao

#endif // TUFAO_THREADEDHTTPREQUESTDISTPATCHER_H
