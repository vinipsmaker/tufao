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

/*!
  \mainpage

  \section tufao-overview Introduction

  Tufão is a web framework for C++ that makes use of Qt's object communication
  system (signals & slots). It features:
    - High performance standalone server
    - Cross-plataform support
    - Good documentation
    - Support modern HTTP features
     - Persistent streams
     - Chunked entities
     - 100-continue status
     - WebSocket
    - HTTPS support
    - Flexible request router
    - Static file server with support for conditional requests and partial
      download
    - Plugin-based server to allow change the running code without restart the
      application
    - Flexible and secure session support
    - QtCreator's plugin to allow create new applications rapidly
    - Timeout support

  It uses Ryan Dahl's HTTP parser to provide good performance.

  \tableofcontents

  \section tufao-introduction Creating a Tufão-based application

  In this section, I assume that you already have installed Tufão libraries and
  tools (tufao-routes-editor, QtCreator's plugin, ...).

  \subsection tufao-hello Hello World!

  ![](get_started_1.png "QtCreator's Tufão plugin")

  First of all, you must open QtCreator and go to the new project dialog, you
  may be able to select Tufão Web Server project from the list of projects in
  the Tufão category.

  ![](get_started_2.png "Tufão Web Server project templates")

  Go through the pages of the project wizard, selecting the _application_
  template. At the end, you'll get an unconfigured qmake-based application.
  Under QtCreator's interface, go to the _projects mode_ and choose a
  configuration. If you don't configure the project, QtCreator won't be able to
  compile your project.

  \subsection tufao-code And then, there was the code

  You may have a _main.cpp_ file with a code like this:

  \includelineno applicationdefaultmain.cpp

  A main file with 50 lines of code. It's a bunch of code, but if you delete the
  comments and includes, you'll have only 30 lines remaining. It's possible to
  create Tufão applications with fewer yet lines, but I'll use this code to
  explain important Tufão features. They'll prove their value.

  Tufão provides a web framework and has its own HTTP server (based on Ryan
  Dahl's HTTP parser). This design give you greater control over the
  applications and can really boost its performance. But, as uncle Ben may have
  told you:

  > With great power comes great responsibility

  This may sound scary to you at first, but don't worry, the Tufão team will do
  their best to make this responsibility easier to assume, with each realease.
  But you need to promisse that you'll use Tufão facilities and read the
  documentation.

  To use the Tufão HTTP server, we instantiate, as show in the line 17, a
  Tufao::HttpServer, put it to listen on port 8080, as show in the line 46, and
  start a event loop, as show in the line 49. This server will expose all we
  need to this introduction.

  The Tufao::HttpServer will emit the Tufao::HttpServer::requestReady signal
  each time a request comes. We can handle all requests in a single place in the
  code, but we won't do that, because... em... as some wise person said before,
  we need:

  > [Divide and conquer](https://en.wikipedia.org/wiki/Divide_and_conquer)

  We could decompose our slot into some functions to do the job. But we won't
  do that, because Tufão already has a better abstraction for the problem,
  Tufao::HttpServerRequestRouter.

  The idea behind Tufao::HttpServerRequestRouter is have a chain of request
  handlers and mapped paths to them. If a request comes to a handler unable to
  handle it, the request is delegated to another handler in the chain.

  In the code, we have one router (line 20) and four handlers (lines 25, 27, 29
  and 31). In the lines 36 to 39 we bind the router and the handlers and in the
  line 42 we bind the router and the server. The handlers, in order, are:

    - Tufao::HttpPluginServer: Uses plugins to handle the requests. A plugin
      mechanism is what allows you to change the running code without restart
      the application. In this code, we use the file routes.conf (editable with
      the tufao-routes-editor tool) to configure the plugins.
    - PluginReloader: A custom class (defined in the files _pluginreloader.h_
      and _pluginreloader.cpp_) that instructs the pluginServer to refresh the
      list of plugins. For security reasons, it'll only accept requests
      originated from local host.
    - Tufao::HttpFileServer: Serve static files. We use the folder _public_ as
      root dir.
    - NotFoundHandler: A custom class (defined in the files _notfound.h_ and
      _notfound.cpp_) that responds to every request with a 404 status code. It
      will use the file _notfound.html_ embedded in the binary (through the Qt
      resource system) as message body of the responses. Line 14 init the
      necessary resource.

  If you run this application, you see the error _Unsupported config file
  version_. This is happening because you didn't prepare a _routes.conf_ file to
  feed the plugin handler. See \ref pluginsystem to learn how to fix it and add
  a plugin-based handler to the application. The file _routes.conf_ must be
  placed in the working dir from where the application will run (or is running).

  If you access http://localhost:8080/ you'll see a message telling you that the
  page wasn't found. Create a folder called public in the working dir of the
  executable and put a file called index.html there:

      <html>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <head><title>Hello World</title></head>
        <body>
          <h1>Congratulations,</h1>
          <p>you are able to start web development in C++ with Tufão</p>
        </body>
      </html>

  Now access the page <http://localhost:8080/index.html> and see the result. As
  an exercise, I left the task to add a
  [favicon](https://en.wikipedia.org/wiki/Favicon) to your application using the
  \ref pluginsystem.

  \subsection tufao-build Build system integration

  The Tufão has native support to three build systems: qmake, pkgconfig and
  CMake.

  Once Tufão is correctly installed, you can use Tufão in your qmake-based
  applications appending the following line to your project file, where _x_
  is the major version (0 for Tufão 0.x) you want to use:

      CONFIG += TUFAOx

  If you use CMake, just follow the common steps (require package and handle
  compile and link flags) in your CMakeLists.txt file:

      find_package(Tufao 0.4 REQUIRED)
      include_directories("${TUFAO_INCLUDE_DIR}")
      target_link_libraries(foobar ${TUFAO_LIBRARIES})

  If you want to use a FindTufao.cmake file embedded in your project dir, add
  the following line to your CMakeLists.txt:

      set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/path/to/FindTufao.cmake")

  If you want to use autotools or another build system that has pkgconfig
  support, you can use the Tufão's pkgconfig module, where _x_ is the major
  version (0 for Tufão 0.x) you want to use:

      $ pkg-config tufaox

  If want to use a build system that don't support pkgconfig, you should be
  ashamed, but you may still be able to use Tufão (probably editing the compiler
  -I flag and the linker -L and -l flags).
  */

/*!
  This is the namespace where all Tufão facilities are grouped. It's purpose is
  isolate its symbols from symbols of other packages, avoiding collision
  problems.
  */
namespace Tufao {}
