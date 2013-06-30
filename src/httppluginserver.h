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

#ifndef TUFAO_HTTPPLUGINSERVER_H
#define TUFAO_HTTPPLUGINSERVER_H

#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

/*!
  This class provides a plugin-based request handler. Use it if you need to
  change the running server code without need to restart the appplication.

  It maintains its own set of rules and is as powerful as
  HttpServerRequestRouter (and internally uses it), but, in contrast, exports
  its configuration through a file and will use handlers loaded from plugins.

  The file use a syntax based on the QSettings ini format and can edit it using
  tufao-routes-editor.

  \note
  If you want to control when the plugins are reloaded, you must ensure that
  autoreload is false and explicitly call the slot
  HttpPluginServer::reloadConfig.

  \sa
  AbstractHttpServerRequestHandlerFactory to implement your plugins.

  \sa
  \ref pluginsystem

  \since
  0.3
  */
class TUFAO_EXPORT HttpPluginServer : public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a HttpPluginServer object.

      \param configFile It's used as configuration file.

      \param parent It's passed to the QObject constructor.

      \warning
      The object created by this constructor doesn't autoreload the config file
      when it changes. You can use another constructor or call
      HttpPluginServer::setConfig if you want a different behaviour.
      */
    explicit HttpPluginServer(const QString &configFile = QString(),
                              QObject *parent = 0);

    /*!
      Constructs a HttpPluginServer object.

      \param configFile It's used as configuration file.

      \param autoreload Whether HttpPluginServer will autoreload the config file
      when it is modified.

      \param parent It's passed to the QObject constructor.

      \since
      0.8
      */
    explicit HttpPluginServer(const QString &configFile, bool autoreload,
                              QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpPluginServer();

    /*!
      This is an overloaded function.

      It will **NOT** autoreload the config referenced by \p file.
      */
    void setConfig(const QString &file);

    /*!
      Set the configuration file used to handle requests.

      Call this function will reload the configuration.

      \p autoreload tells whether HttpPluginServer should autoreload the config
      file when it is modified.

      \sa
      config

      \since
      0.8
      */
    void setConfig(const QString &file, bool autoreload);

    /*!
      Return the current used configuration file. This file is used to handle
      requests, loading the appropriate plugins, generating actual handlers and
      mapping them to the rules described in this file.
      */
    QString config() const;

public slots:
    /*!
      Handle the request using the loaded plugins and rules.
      */
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList &args = QStringList());

    /*!
      Clear all previous mappings and reload all rules and plugins.

      If you want to control when the plugins are reloaded, you must ensure that
      autoreload is false and explicitly call this slot when you want to make
      the changes of the config file take effect.

      This is an advanced use of HttpPluginServer and you probably just want to
      ensure autoreload is true to avoid unnecessary complexities.
      */
    void reloadConfig();

private slots:
    void onConfigFileChanged();

private:
    void clear();

    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPPLUGINSERVER_H
