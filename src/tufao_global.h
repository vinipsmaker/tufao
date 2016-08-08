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

#ifndef TUFAO_GLOBAL_H
#define TUFAO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TUFAO_LIBRARY)
#  define TUFAO_EXPORT Q_DECL_EXPORT
#else
#  define TUFAO_EXPORT Q_DECL_IMPORT
#endif

#ifndef TUFAO_VERSION_MAJOR
#  define TUFAO_VERSION_MAJOR 1
#endif

#ifndef TUFAO_VERSION_MINOR
#  define TUFAO_VERSION_MINOR 4
#endif

#endif // TUFAO_GLOBAL_H
