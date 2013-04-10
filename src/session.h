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
 * This class provides easier access to the session's properties. It uses C++
 * features used in containers to provide a familiar interface, such as
 * overloading the operator [].
 *
 * \include sessionusage.cpp
 *
 * \note
 * All member functions of this class are inlined and should add the minimum (if
 * any) of overhead.
 *
 * \sa
 * SessionStore
 *
 * \since
 * 0.4
 */
class TUFAO_EXPORT Session
{
public:
    /*!
     * Provides a object that give less verbose access to a session property.
     *
     * \note
     * You shouldn't create it directly, but use Session::operator [].
     *
     * \sa
     * Session
     */
    class PropertyWrapper
    {
    public:
        /*!
         * Constructs a new PropertyWrappe object.
         *
         * \note
         * This object will use \p session in every operation and you should
         * ensure that \p session isn't destructed before the PropertyWrapper
         * object.
         */
        PropertyWrapper(Session &session, const QByteArray &key) :
            session(session),
            key(key)
        {}

        /*!
         * Returns true if this property exists for this session.
         */
        operator bool() const
        {
            return session.hasValue(key);
        }

        /*!
         * Assigns \p value to this property and returns a reference to itself.
         */
        PropertyWrapper &operator =(const QVariant &value)
        {
            session.setValue(key, value);
            return *this;
        }

        /*!
         * Returns this property's value.
         */
        QVariant operator ()()
        {
            return session.value(key);
        }

    private:
        Session &session;
        const QByteArray key;
    };

    /*!
     * Constructs a new Session object.
     *
     * The object will use \p store, \p request and \p response in every
     * operation and you should ensure these objects aren't destructed before
     * this Session object.
     */
    Session(SessionStore &store, const HttpServerRequest &request,
            HttpServerResponse &response) :
        store(store),
        request(request),
        response(response)
    {}

    /*!
     * Returns true if the session has a property accessible through \p key.
     *
     * \sa
     * SessionStore::hasProperty
     */
    bool hasValue(const QByteArray &key) const
    {
        return store.hasProperty(request, response, key);
    }

    /*!
     * Returns the value of the property referenced by \p key, or a null
     * QVariant if the property isn't found.
     *
     * \sa
     * SessionStore::property
     */
    QVariant value(const QByteArray &key) const
    {
        return store.property(request, response, key);
    }

    /*!
     * Sets the property's value referenced by \p key to \p value.
     *
     * \sa
     * SessionStore::setProperty
     */
    void setValue(const QByteArray &key, const QVariant &value)
    {
        store.setProperty(request, response, key, value);
    }

    /*!
     * Returns a PropertyWrapper that will remember the \p key used to
     * manipulate the session property.
     */
    PropertyWrapper operator [](const QByteArray &key)
    {
        return PropertyWrapper(*this, key);
    }

    /*!
      Takes a functor to access a session's property.

      \param f a functor that receives a QVariant object reference as an
      argument.

      \note After the functor returns, the property is updated.

      \include sessionusage2.cpp

      \since
      1.0
    */
    template<class F>
    static void apply(SessionStore &store, const QByteArray &property,
                      const HttpServerRequest &request,
                      HttpServerResponse &response, F f)
    {
        QVariant v = store.property(request, response, property);
        f(v);
        store.setProperty(request, response, property, v);
    }

    /*!
      Takes a functor to access the session's properties.

      \param f a functor that receives a QMap<QByteArray, QVariant> object
      reference as an argument.

      \note After the functor returns, the property is updated.

      \include sessionusage3.cpp

      \note
      If the session may contain a lot of properties and you aren't going to
      access most of them, this helper function might be inefficient.

      \since
      1.0
    */
    template<class F>
    static void apply(SessionStore &store, const HttpServerRequest &request,
                      HttpServerResponse &response, F f)
    {
        QMap<QByteArray, QVariant> properties;

        for (const auto &property: store.properties(request, response))
            properties[property] = store.property(request, response, property);

        f(properties);

        for (auto i = properties.begin();i != properties.end();++i)
            store.setProperty(request, response, i.key(), i.value());
    }

private:
    SessionStore &store;
    const HttpServerRequest &request;
    HttpServerResponse &response;
};

} // namespace Tufao

#endif // TUFAO_SESSION_H
