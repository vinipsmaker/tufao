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

#ifndef TUFAO_SESSIONSTORE_H
#define TUFAO_SESSIONSTORE_H

#include "httpserverrequest.h"
#include "sessionsettings.h"

namespace Tufao {

/*!
 * SessionStore class can be used to store data that must persist among
 * different requests. This is used in web applications, among other, to measure
 * online users, create a shopping list per user and implement a login system.
 *
 * Using sessions
 * ==============
 *
 * The session data is stored in properties. You can create a session to any
 * pair of request and response objects and each one will have an independent
 * set of properties.
 *
 * \note
 * You can configure a session in any response (including 400-level and
 * 500-level status responses), but the user agent may ignore sessions with a
 * 100-level status line.
 *
 * To access the session properties, you must pass the request and response
 * objects each time you want to manipulate, but this step can be eliminated
 * using the Session helper class.
 *
 * \note
 * You can configure multiple sessions to the same user agent, but only if you
 * are using settings that have different scope. The settings have the same
 * scope if the name, domain and path attributes are equal.
 *
 * \warning
 * Despite the possibility of use different stores having SessionSetting objects
 * with the same name attribue, it's recomended to avoid the use of this feature
 * because a store don't have knowledge about the settings of other stores and
 * can easily use the wrong request's cookie in a misconfigured application.
 *
 * One session only persists for a specified period of time (set through
 * SessionSettings) and the storage details depends upon the class implementing
 * the SessionStore interface.
 *
 * \note
 * The use of sessions don't preclude HTTP caches from storing and reusing a
 * response.
 *
 * Implementing your own storage backend
 * =====================================
 *
 * If the implementations shipped with Tufão don't supply your needs, you can
 * provide your own storage backend. To do that, you must implement the pure
 * virtual methods of this class. They give you full access to the cookies
 * returned from the user agent and included in the response objects.
 *
 * The protected methods may help you process and include the cookies. They
 * already take care of the boring task of read and comply with the
 * SessionSettings object.
 *
 * This flexible design is what allows you, among other, implement a pure cookie
 * based storage mechanism.
 *
 * \sa
 * SessionSettings Session
 *
 * \since 0.4
 */
class TUFAO_EXPORT SessionStore : public QObject
{
    Q_OBJECT
public:
    /*!
     * Constructs a SessionStore object.
     *
     * \par settings Specifies session parameters.
     * \par parent is passed to the QObject constructor.
     */
    explicit SessionStore(const SessionSettings &settings = defaultSettings(),
                          QObject *parent = 0);

    /*!
     * Returns true if a session has been set the \p request.
     */
    virtual bool hasSession(const HttpServerRequest &request) const = 0;

    /*!
     * Removes the session, if any, in the \p request.
     *
     * \par response The object used to invalidate the user's cookie.
     */
    virtual void removeSession(const HttpServerRequest &request,
                               HttpServerResponse &response) = 0;

    /*!
     * Returns a list of set properties to this session.
     *
     * The session is represented by the pair \p request, \p response.
     */
    virtual QList<QByteArray> properties(const HttpServerRequest &request,
                                         const HttpServerResponse &response)
        const = 0;

    /*!
     * Returns true if the session has a property named \p key.
     *
     * The session is represented by the pair \p request, \p response.
     */
    virtual bool hasProperty(const HttpServerRequest &request,
                             const HttpServerResponse &response,
                             const QByteArray &key) const = 0;

    /*!
     * Returns the property referenced by \p key in the session, or a null
     * QVariant if the property isn't found.
     *
     * The session is represented by the pair \p request, \p response.
     */
    virtual QVariant property(const HttpServerRequest &request,
                              HttpServerResponse &response,
                              const QByteArray &key) const = 0;

    /*!
     * Sets the property's value referenced by \p key to \p value.
     *
     * The session is represented by the pair \p request, \p response.
     */
    virtual void setProperty(const HttpServerRequest &request,
                             HttpServerResponse &response,
                             const QByteArray &key, const QVariant &value) = 0;

    /*!
     * Removes the property referenced by \p key in the session, if any.
     *
     * The session is represented by the pair \p request, \p response.
     */
    virtual void removeProperty(const HttpServerRequest &request,
                                HttpServerResponse &response,
                                const QByteArray &key) = 0;

    /*!
     * Returns the default settings, used when you don't specify the settings
     * argument in SessionStore constructor.
     *
     * The default values to these settings are:
     *   - timeout: 15
     *   - httpOnlye: true
     *   - key: "SID"
     *   - path: "/"
     *   - secure: false
     *
     * If you use more than one store in the same application, you need to set a
     * unique key to each one.
     */
    static SessionSettings defaultSettings();

protected:
    /*!
     * Returns the value of the first cookie that is compatible with this
     * store's properties, as defined in its settings.
     */
    QByteArray session(const HttpServerRequest &request) const;

    /*!
     * Returns the value of the first cookie that is compatible with this
     * store's properties, as defined in its settings.
     *
     * Besides searching in the \p request's headers named as "Cookie", search
     * in \p response's headers named as "Set-Cookie". This allows you to
     * manipulate sessions as soon as they are set.
     */
    QByteArray session(const HttpServerRequest &request,
                       const HttpServerResponse &response) const;

    /*!
     * Sets a cookie that matches the store's settings in the \p response
     * object.
     *
     * \note
     * It will also renew cokie's lifetime.
     *
     * \warning
     * Call this method more than once to the same \p response object has
     * undefined behaviour.
     *
     * \note
     * If you need to create a new unique identifier, but don't know how, check
     * the QUuid class.
     *
     * \sa
     * SessionSettings::cookie
     */
    void setSession(HttpServerResponse &response, const QByteArray &session)
        const;

    /*!
     * Invalidates the user's session.
     *
     * This is done by setting the expiration date to the past, as specified by
     * RFC 6265.
     */
    void unsetSession(HttpServerResponse &response) const;

    /*!
     * This attribute represents the session's settings. It will be used in
     * the operations involving cookie's handling and is set automatically in
     * SessionStore's constructor.
     */
    SessionSettings settings;
};

} // namespace Tufao

#endif // TUFAO_SESSIONSTORE_H
