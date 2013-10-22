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

#ifndef TUFAO_THREADEDHTTPPLUGINSERVER_H
#define TUFAO_THREADEDHTTPPLUGINSERVER_H

#include <functional>
#include <initializer_list>

#include <QtCore/QRegularExpression>
#include <QtCore/QObject>
#include <QtCore/QDebug>

#include "tufao_global.h"
#include "threadedhttpserver.h"


namespace Tufao {

class ThreadedHttpPluginServer : public ThreadedHttpServer
{
    Q_OBJECT

public:

    explicit ThreadedHttpPluginServer(QObject *parent = 0);


    /*!
     * \brief setConfig
     * \param file
     * \return
     * \sa HttpPluginServer::setConfig
     *
     * Loads the config file and starts the internal threads.
     */
    bool setConfig(const QString &file);

    /*!
      Returns the path of the last configuration file used. This file is used to
      handle requests, loading the appropriate plugins, generating actual
      handlers and mapping them to the rules described in this file.

      \sa
      setConfig
      */
    QString config() const;

private:
    void changeConfig(const QString &file);
    void onConfigFileChanged();
    void clear();
    void loadConfig();

    struct Priv;
};

} // namespace Tufao

#endif // TUFAO_THREADEDHTTPPLUGINSERVER_H
