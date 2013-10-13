# Coding Style

You should follow the
[Qt Coding Style](http://qt-project.org/wiki/Qt_Coding_Style). Exceptions:

  * In Qt, public classes start with a 'Q'. Tufão uses namespace to group
    declarations. You **MUST** put public declarations under _Tufao_ and public
    macros **MUST** start with _TUFAO__.
  * Try to keep lines with less than or equal to 80 characters.

# Coding Conventions

Tufão has its own coding conventions, apart from
[Qt Coding Conventions](http://qt-project.org/wiki/Coding-Conventions). The
following rules **MUST** be respected:

  * All code is UTF-8 only
  * The only Tufão dependency is Qt and use native/low-level API is **not**
    allowed
  * Always use this form to include Qt headers:

        #include <QtCore/QWhatever>

  * Always use quotes-enclosed form to include other Tufão headers:

        #include "othertufaoheader.h"

    The corresponding &lt;OtherTufaoHeader&gt; is only available after the
    installation.
  * Private headers **MUST** be put under the _src/priv_ folder. Otherwise they
    will be included for installation.
  * C++11 is allowed for Tufão 1.x and later.

## Rules inherited from _Qt Coding Conventions_

  * Every QObject subclass must have a Q_OBJECT macro, even if it doesn’t have
    signals or slots, otherwise qobject_cast will fail.
  * Normalize the arguments for signals + slots (see
    QMetaObject::normalizedSignature) inside connect statements to get faster
    signal/slot lookups. You can use $QTDIR/util/normalize to normalize existing
    code.

### Conventions for public header files

  * Definitions:
    * Qt 4.0.0 is a major release, Qt 4.1.0 is a minor release, Qt 4.1.1 is a
      patch release
    * Backward binary compatibility: Code linked to an earlier version of the
      library keeps working
    * Forward binary compatibility: Code linked to a newer version of the library
      works with an older library
    * Source code compatibility: Code compiles without modification
  * Keep backward binary compatibility + backward source code compatibility in
    minor releases
  * Keep backward and forward binary compatibility + forward and backward source
    code compatibility in patch releases
    * Don’t add/remove any public API (e.g. global functions,
      public/protected/private methods)
    * Don’t reimplement methods (not even inlines, nor protected/private methods)

# Documentation

  * Classes undocumented **MUST NOT** be part of a release.
  * Documentation **MUST** be in [Doxygen](http://doxygen.org/)'s syntax and
    **MUST** be put in the headers. Documentation from source files are not
    generated.
  * Use _\command_ instead of _@command_.
  * _\include_ files **MUST** be placed under _src/doc/examples_.
  * Image files **MUST** be placed under _src/doc/img_
  * You can put documentation unrelated to a specific class using the _\page_
    command. You shall put one page per file and these type of files **MUST** be
    placed under _src/doc_.
  * If a signal is unsafe, you **must** document it:

        \note
        __This signal is unsafe__ (read this: \ref safe-signal)!

# Introducing a new feature

  1. Add the function signature in an existing header.
  2. If the function is not member of any existing class, use the TUFAO_EXPORT
     macro in its signature. This macro is available under
     _src/tufao\_global.h_.
  3. Do not forget to put the Doxygen's _\since_ command in the documentation.
  4. Implement the function in the corresponding source file.
  5. Create tests
  6. Document the change in some place.

## Introducing a new class

  1. Create the header file.
     * It **MUST** contain a copyright notice and be distributed under the MIT
       license.
     * It **MUST** use a define guard.
  2. Put the class declaration. If it's not a template, then it **MUST** use
     the TUFAO_EXPORT macro. This macro is available under
     _src/tufao\_global.h_.
  3. Document the class. And **DON'T FORGET** to use the _\since_ command.
  4. Create the source file.
     * It **MUST** contain a copyright notice and be distributed under "LGPLv2.1
       or (at your option) any later version"
  5. Put the source file reference to the variable _tufao_SRC_ under
     _src/CMakeLists.txt_.
  6. Do not forget to put one file in the _include_ folder
  7. Create tests
  8. Create an example

# Tests

Tufão uses Qt Test to create tests. To create a Qt Test based tests, just
subclass QObject and add one or more private slots. Each slot is a test.

To run the test, use the macro _QTEST_MAIN_ in the test class source file. This
macro define a main function.

Each test generate an executable. CTest is used to run each executable and
collect the results.

## Simple

Your test is simple if it meets the following requirements:

  * Only generates one object (eg. one class only).
  * It doesn't have extra dependencies.
  * It doesn't need special build configurations (eg. Qt Resource system, ...).

If your test is simple, just put its source file (and corresponding header)
under _src/tests_ and add the name of the file (without the extension) to the
variable _tests_ in _src/tests/CMakeLists.txt_.

## Complex

If your test is complex, create a folder containing its build settings under
_src/tests_ and add the name of the file (the name of the folder) to the
variable _tests_ in _src/tests/CMakeLists.txt_.

Use the setup_test_target macro to configure the target. This macro is defined
under _src/tests/CMakeLists.txt_.

# Repository

Individual features being developed may have its own branches to avoid blocking
releases. When a feature is ready (functionality, documentation, tests and
examples), it can be merged in the _master branch_.

# Send your changes

Did you make a modification to Tufão and think its worth to go upstream?

  * Send a [pull request](http://archlinux.me/dusty/2012/03/18/contributing-to-git-projects-on-github/)
  * Send the patch to the [mailing list](https://github.com/vinipsmaker/tufao/wiki/Contact)
    (tip: use the command _git format-patch_ to generate the patches)

# Preparing a new release

  * It's stable ;)
    * Correctly compile on all plataforms
    * New features are working
    * Old features continue to work
  * Every new feature is correctly documented with a _\since_ command
  * The new features are documented under the release notes of _README.md_ file
  * There are examples of the new features under _examples_ folder (optional)
  * Put a new application template to demonstrate the new features on
    _tufao-qtcreator-plugin_ (optional)
  * Create a commit to bump the release version (see script
    _scripts/change-version.sh_)
  * Create a signed git tag
  * Sync the online documentation to reflect the new release
    * Do **NOT** include the _html/index.qhp_ file
  * Announce the release on the mailing list
  * If the release is special (e.g. a major version bump), announce it on KDE
    and Qt forums too
  * Party time!
