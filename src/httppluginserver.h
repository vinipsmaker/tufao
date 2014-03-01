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

#include <QtCore/QObject>

#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

/*!
  This class provides a plugin-based request handler. Use it if you need to
  change the application code without restart the server.

  It maintains its own set of rules and is as powerful as
  HttpServerRequestRouter (and internally uses it), but, in contrast, exports
  its configuration through a json file and will use handlers loaded from
  plugins.

  The file format is described in the HttpPluginServer::setConfig method.

  \note
  The object will monitor the config file for changes and reload it as
  changes happens.

  \sa
  AbstractHttpServerRequestHandlerFactory to implement your plugins.

  \sa
  \ref pluginsystem

  \since
  0.3
  */
class TUFAO_EXPORT HttpPluginServer : public QObject, 
                                      public AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    /*!
      Constructs a null HttpPluginServer object.

      \p parent is passed to the QObject constructor.
      */
    explicit HttpPluginServer(QObject *parent = 0);

    /*!
      Constructs a HttpPluginServer object.

      \p parent is passed to the QObject constructor.

      \p configFile is used as configuration file.

      \sa
      setConfig
      */
    explicit HttpPluginServer(const QString &configFile, QObject *parent = 0);

    /*!
      Destroys the object.
      */
    ~HttpPluginServer();

    /*!
      Set the configuration file used to handle requests. After the file is set,
      HttpPluginServer will watch the file for changes and reload its config
      when the file changes.

      \warning
      HttpPluginServer can't monitor file links correctly.

      \note
      The old config is cleared even if it fails to set the new config.

      \retval true if HttpPluginServer finds the file.
      \retval false if HttpPluginServer can't find the file.

      Call this method with an empty string if you want to _clear_ the plugin
      server.

      The HttpPluginServer behaviour
      ==============================

      An simplified use case to describing how HttpPluginServer reacts to
      changes follows:

      1. You start with a default-constructed HttpPluginServer
      2. You use setConfig with an inexistent file
       1. The HttpPluginServer do not find the file
       2. HttpPluginServer::setConfig returns false
       3. HttpPluginServer object remains in the previous state
      3. You use setConfig with a invalid file
       1. HttpPluginServer starts to monitor the config file
       2. HttpPluginServer::setConfig returns true
       3. HttpPluginServer reads the invalid file and remains in the previous
          state.
      4. You fill the config file with a valid config.
       1. HttpPluginServer object load the new contents
       2. HttpPluginServer try to load every plugin and fill the router. If a
          plugin cannot be loaded, it will be skipped and a warning message is
          sent through qWarning. If you need to load this plugin, make any
          modification to the config file and HttpPluginServer will try again.
      5. You fill the config file with an invalid config.
       1. HttpPluginServer see and ignores the changes, remaining with the
          previous settings.
      6. You remove the config file.
       1. HttpPluginServer object come back to the default-constructed state.

          <h2>version: 0</h2>

          If the last config had "version: 0", then it means no more monitoring
          either (this is what default-constructed state means).

          <h2>version: 1</h2>

          If the last config had "version: 1", then HttpPluginServer will (after
          the cleanup) start to monitor the containing folder, waiting until a
          config file with the same name is available again to resume its
          operation.

          \note
          A later call to HttpPluginServer::setConfig can be used to stop the
          monitoring.

          &nbsp;

          \note
          If the containing dir is also erased, HttpPluginServer can do nothing
          and the monitoring will stop.

      &nbsp;

      The file format
      ===============

      The configuration file format is json-based. If you aren't used to JSON,
      read the [json specification](http://json.org/).

      \note
      The old Tufão 0.x releases used a file with the syntax based on the
      QSettings ini format and forced you to use the _tufao-routes-editor_
      application to edit this file.

      The file must have a root json object with 3 attributes:

      - _version_: It must indicate the version of the configuration file. The
        list of acceptable values are:
       - _0_: Version recognizable by Tufão 1.x, starting from 1.0
       - _1_: Version recognizable by Tufão 1.x, starting from 1.2. The only
         difference is the autoreloading behaviour. If you delete the config
         file, Tufão will start to monitor the containing folder and resume the
         normal operation as soon as the file is added to the folder again.
      - _plugins_: This attribute stores metadata about the plugins. All plugins
        specified here will be loaded, even if they aren't used in the request
        router. The value of this field must be an array and each element of
        this array must be an object with the following attributes:
       - _name_: This is the name of the plugin and defines how you will refer
         to this plugin later. You can't have two plugins with the same name.
         This attribute is **required**.
       - _path_: This is the path of the plugin in the filesystem. Relative
         paths are supported, and are relative to the configuration file. This
         attribute is **required**.
       - _dependencies_: This field specifies a list of plugins that must be
         loaded before this plugin. This plugin will be capable of access
         plugins listed here. This attribute is **optional**.
       - _customData_: It's a field whose value is converted to a QVariant and
         passed to the plugin. It can be used to pass arbitrary data, like
         application name or whatever. This attribute is **optional**.
      - _requests_: This attribute stores metadata about the requests handled by
        this object. The value of this field is an array and each element of
        this array describes a handler and is an object with the following
        attributes:
       - _path_: Defines the regex pattern used to filter requests based on the
         url's path component. The regex is processed through
         QRegularExpression. This attribute is **required** and **must** be an
         valid regex.
       - _plugin_: Defines what plugin is used to handle request matching the
         rules defined in this containing block. This attribute is **required**.
       - _method_: Define what HTTP method is accepted by this handler. This
         field is **optional** and, if it's not defined, it won't be used to
         filter the requests.

      \note
      An empty value isn't acceptable to a field, except for _customValue_ at
      _plugins_ and _path_ at _requests_ fields. If you use an unacceptable
      value, Tufão may reject your file.

      An example follows:

          {
              version: 1,
              plugins: [
                  {
                      name: "home",
                      path: "/home/vinipsmaker/Projetos/tufao-project42/build/plugins/libhome.so",
                      customData: {appName: "Hello World", root: "/"}
                  },
                  {
                      name: "user",
                      path: "show_user.so",
                      dependencies: ["home"]
                  },
                  {
                      name: "404",
                      path: "/usr/lib/tufao/plugins/notfound.so",
                      customData: "<h1>Not Found</h1><p>I'm sorry, but it's your fault</p>"
                  }
              ],
              requests: [
                  {
                      path: "^/$",
                      plugin: "home",
                      method: "GET"
                  },
                  {
                      path: "^/user/(\w*)$",
                      plugin: "user"
                  },
                  {
                      path: "",
                      plugin: "404"
                  }
              ]
          }

      The _requests_ attribute is used to seed data to a HttpServerRequestRouter
      object. Because this, you can use features like return false from a
      handler to allow another handler handle a request.

      \note
      If the HttpPluginServer finds an attribute not recognizable, the attribute
      will be skipped. You can use this to extend the file with customized
      fields and the HttpPluginServer will continue to behave normally.

      \warning
      After reload the config, Tufão might recycle some objects to achieve the
      goal of build a matching request router faster, if dependencies don't
      change. You can rely on the dependency system for initialization order,
      but you can't know if the plugins will or not be reloaded after the config
      changes, because the behaviour is not defined and might change in
      different Tufão versions.

      \sa
      config
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

public slots:
    /*!
      Handle the request using the loaded plugins and rules.

      \note
      In Tufão 0.x, handleRequest caught exceptions thrown by plugins, but in
      Tufão 1.0, this behaviour is not used anymore, because it was stealing the
      power and the control of the programmer.

      \since
      1.0
      */
    bool handleRequest(Tufao::HttpServerRequest &request,
                       Tufao::HttpServerResponse &response) override;

private:
    void changeConfig(const QString &file);
    void onConfigFileChanged();
    void onConfigDirChanged();
    void clear();
    void reloadConfig();

    struct Priv;
    Priv *priv;
};

} // namespace Tufao

#endif // TUFAO_HTTPPLUGINSERVER_H
