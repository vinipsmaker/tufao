/*
  Copyright (c) 2015 Vinícius dos Santos Oliveira

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
  \page design-model Design Model

  This page describes Tufão's design model.

  Just like Node.JS, Tufão uses an async event-driven model. That is, Tufão will
  handle multiple connections per thread. The Qt executor
  (i.e. `QCoreApplication`, `QThread` or other) will take care of the scheduling
  tasks required to handle multiple connections per thread. You can also spawn
  multiple executors in different threads and delegate different handlers to
  different executors, then you'll have a
  multiple-connections-per-event-loop/one-event-loop-per-thread architecture.

  The server will begin to work once the control goes to the Qt executor
  (e.g. `QCoreApplication::exec`).

  The simplest choice is just to use single-threaded async I/O.
 */
