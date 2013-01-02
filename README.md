# Tufão - an asynchronous web framework for C++ built on top of Qt

Tufão is a web framework for C++ that makes use of Qt's object communication
system (signals & slots). It features:
  * High performance standalone server
  * Cross-plataform support
  * Good [documentation](http://vinipsmaker.github.com/tufao/ref/)
  * Support modern HTTP features
   * Persistent streams
   * Chunked entities
   * 100-continue status
   * WebSocket
  * HTTPS support
  * Flexible request router
  * Static file server with support for conditional requests and partial
    download
  * Plugin-based server to allow change the running code without restart the
    application
  * Flexible and secure session support
  * QtCreator's plugin to allow create new applications rapidly
  * Timeout support

It uses Ryan Dahl's HTTP parser to provide good performance.

You can generate documentation from the source code using Doxygen. The
documentation will be put on the doc folder in the html and tex formats. There
is also experimental support for Qt compressed help files.

## LICENSE

The library is under the LGPLv2 and public header files, documentation and
examples are under MIT license.

The library is dynamic linked against Qt and include code from Ryan Dahl's HTTP
parser. Qt library is licensed under LGPL and Ryan Dahl's HTTP parser is
licensed under MIT license.

So, you can create commercial applications (the *only* restriction is that *if*
you do any modifications to *Tufão*, these modifications must be redistributed).

## BUILD

Make sure you have Qt and CMake installed and with the PATH to its executables
set, then create a folder for the build and, from there, run:

    $ cmake OPTIONS path_to_source_dir
    $ make MAKEOPTIONS

OPTIONS can be null or have a combination of the following values:
  * -DCMAKE_INSTALL_PREFIX=${INSTALLDIR} sets the directory where to install
    Tufão.
  * -DCMAKE_BUILD_TYPE=Debug
  * -DCMAKE_BUILD_TYPE=Release
  * -DCMAKE_BUILD_TYPE=RelWithDebInfo
  * -DCMAKE_BUILD_TYPE=MinSizeRel
  * -DGENERATE_DOC=YES generate documentation using Doxygen

OPTIONS available on Windows:
  * -G"MinGW Makefiles" to generate Makefiles for use with MinGW environment
  * -G"Visual Studio 10" to generate project files for Visual Studio 10

MAKEOPTIONS can be null or have a combination of the following values:
  * install installs Tufão
  * DESTDIR=${PKGDIR} sets the directory where to install Tufão. This options
    should be used if you intend to package Tufão to set the package directory.
    To choose another installation directory, see options in OPTIONS, above.

Example:

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr
    $ make DESTDIR=pkg install

NOTE:
Qt 4.7 or later is required.

To generate the documentation, just run doxygen using Doxyfile as configuration
file and the documentation will be generated in the folder doc. The
documentation is available in the following formats:
  * HTML: Always generated. It should be in the doc/html folder.
  * latex: You can use this format to generate a pdf. Just run make inside the
    doc/latex folder.
  * Qt Compressed Help file: If you have qhelpgenerator binary tool in the
    system PATH, then the file is generated when you run doxygen and should be
    in doc/qch/tufao.qch. If you have qhelpgenerator tool installed, but not
    configured in the system PATH, but still want to generate the documentation
    in this format, just run the tool using doc/html/index.qhp as input file.

Tufão also have a lot of code to test its correctness. These codes are seperated
in one different qmake project for each Tufão class and are located in the
folder tests. If you got Tufão from a git checkout (not a stable release), then
you may be interessed in run these tests. To compile and run them, just run the
execute-tests.sh script.

NOTE:
If you intend to create a CPack-based installer, just run:

    # To create a binary distribution:
    cpack -C CPackConfig.cmake

    # To create a source distribution:
    cpack -C CPackSourceConfig.cmake

    # To create a Windows NSIS-based installer:
    cpack -GNSIS

## INSTALL

The default install prefix is /usr/local, if you don't intend to change it, just
run, after build:

    # make install

To install to a different prefix, run:

    $ cmake -DCMAKE_INSTALL_PREFIX=${DESTDIR}
    $ make install

## USAGE

To use Tufão in your Qt projects, just edit your .pro file and add the line:

> CONFIG += TUFAO0

You can find some examples in the examples folder.

Tufão has PKG-CONFIG support also. Its module name is 'tufao0'.

If you want use Tufão in other build system, just add the compiler option
-ltufao.

You can also see Tufão documentation integrated in QtAssistant.

## NEWS

Version 0.4:
  * Using CMake build system
   * Changes to allow parallel Tufão installations (if major versions differs)
   * Added PKGCONFIG support
  * Session support
  * Better documentation

Version 0.3
  * More application templates in QtCreator Tufão's plugin
  * Class to serve static files with support for conditional requests and
    byte-range requests
  * Robust request router added
  * HTTP plugin server added
  * Using newer version of Ryan Dahl's HTTP parser
  * Changed license from public headers to MIT
  * MSVC support
  * Some minor improvements

Version 0.2:
  * Code is more stable
  * Documentation improved
  * HttpServerResponse is easier to use
  * WebSocket support
  * TUFAO_VERSION_MAJOR and TUFAO_VERSION_MINOR macros added
  * QtCreator plugin
  * New examples

## ROADMAP

Here is a small roadmap for Tufão:

0.5:
  * Better (and easier) multi-thread support

0.6:
  * RPC support
  * Service discovery and description support

1.1:
  * Forms and file uploads
