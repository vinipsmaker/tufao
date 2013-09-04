/*
  Copyright (c) 2013 Benjamin Zeller

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

  The dispatcher supports asynchronous programming and a thread will be attached to a
  request until finished() was emitted from the response object, or the connection is
  closed.

  \warning
  Always call end() on your response object or the thread will be stuck forever with
  the request and the dispatcher will run out of threads.


  The code below provides an example usage:

  \include threadedapplicationdefaultmain.cpp

  \since
  1.0
  */
class TUFAO_EXPORT ThreadedHttpRequestDispatcher: public QObject,
                                            public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
     It's a simple typedef for the type of factory accepted by the
     ThreadedHttpRequestDispatcher.
     */
    typedef std::function<AbstractHttpServerRequestHandler* (void **customData)>
    Factory;

    /*!
     It's a simple typedef for the type of cleanup function accepted by the
     ThreadedHttpRequestDispatcher.
     */
    typedef std::function<void (void **customData)>
    CleanupFunc;

    /*!
      Constructs a ThreadedHttpRequestDispatcher object.
      */
    explicit ThreadedHttpRequestDispatcher(Factory threadInitializer, QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~ThreadedHttpRequestDispatcher();

    /*!
      \brief setCleanupFunction
      Sets the cleanup function that is called when the thread is shutdown
      \note changing this after start was called requires to call restart()
     */
    void setCleanupFunction (CleanupFunc fun);

    /*!
      \brief setThreadPoolSize
      Sets the number of threads that handle incoming requests
      \param size
      \note changing this after start was called requires to call restart()
     */
    void setThreadPoolSize (const unsigned int size);

    /*!
      \brief restart
      Will unload all threads and restart them

      \note This will block until all threads are restarted
     */
    void restart ();

    /*!
      \brief shutdown
      Will stop all threads and request dispatching

      \note This will close all open requests
     */
    void shutdown ();

// QObject interface
public:
    virtual bool event(QEvent *e);

public slots:
    /*!
      Will dispatch the request to the right handler thread.

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

protected slots:
    virtual void initializeThreads();

public:
    struct Priv;

protected:
    explicit ThreadedHttpRequestDispatcher(Priv* priv, Factory threadInitializer, QObject *parent = 0);
    Priv* _priv();
    const Priv *_priv() const;

private:
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
