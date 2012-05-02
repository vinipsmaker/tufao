/*  This file is part of the Tufão project
    Copyright (C) 2011-2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
  \mainpage

  \section introduction Introduction

  Tufão is a web framework for C++ that makes use of Qt's object communication
  system (signals & slots). It features:
    - high performance standalone server
    - Cross-plataform
    - Support modern HTTP features (persistent streams, chunked entities, ...)
    - HTTPS support
    - Lots of tests
    - Timeout support

  It uses a patched version of Ryan Dahl's HTTP parser to provide good
  performance. The patch provides better integration with Qt to provide better
  cross-plataform support and can be found in src/priv/http_parser.patch.

  \section usage Get started

  You should start by taking a look on Tufao::HttpServer documentation or one of
  the examples.

  \note
  Remember to add <em>CONFIG += TUFAO</em> in your *.pro.
  */

/*!
  This is the namespace where all Tufão facilities are grouped.
  */
namespace Tufao {}
