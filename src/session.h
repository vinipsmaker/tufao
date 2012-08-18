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

#ifndef TUFAO_SESSION_H
#define TUFAO_SESSION_H

#include "sessionstore.h"
#include <QtCore/QVariant>

namespace Tufao {

/*!
  \note
  All member functions of this class are inlined and should add the minimum (if
  any) of overhead.
  */
class TUFAO_EXPORT Session
{
public:
    class Wrapper
    {
    public:
        Wrapper(Session &session, const QByteArray &key) :
            session(session),
            key(key)
        {}

        operator bool() const
        {
            return session.hasValue(key);
        }

        operator QVariant() const
        {
            return session.value(key);
        }

        Wrapper &operator =(const QVariant &value)
        {
            session.setValue(key, value);
            return *this;
        }

        QVariant operator ()()
        {
            return static_cast<QVariant>(*this);
        }

    private:
        Session &session;
        const QByteArray &key;
    };

    Session(SessionStore &store, const HttpServerRequest &request,
            HttpServerResponse &response) :
        store(store),
        request(request),
        response(response)
    {}

    bool hasValue(const QByteArray &key) const
    {
        store.hasProperty(request, response, key);
    }

    QVariant value(const QByteArray &key) const
    {
        return store.property(request, response, key);
    }

    void setValue(const QByteArray &key, const QVariant &value)
    {
        store.setProperty(request, response, key, value);
    }

    Wrapper operator [](const QByteArray &key)
    {
        return Wrapper(*this, key);
    }

private:
    SessionStore &store;
    const HttpServerRequest &request;
    HttpServerResponse &response;
};

} // namespace Tufao

#endif // TUFAO_SESSION_H
