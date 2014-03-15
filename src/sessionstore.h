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
 * Security concerns
 * ----------------------
 *
 * \warning
 * Tufão uses cookies as base for its session support, but cookies have a number
 * of security pitfalls and you should read this section if you are going to use
 * them to store any sensitive information, such as login systems.
 *
 * To better understand the problems shown in the following subsections, you
 * should understand how cookies works.
 *
 * A cookie is a piece of text built of a name and a value. The server includes
 * a _Set-Cookie_ header and the user agent will include this cookie in every
 * subsequent request made to this server (when the cookie's scope applies),
 * until the cookie expires.
 *
 * For example, consider that user _U_ made a request to server _S_ and received
 * the following reply:
 *
 *     HTTP/1.1 200 OK
 *     Set-Cookie: Pants=On
 *     ...
 *
 * Then, user _U_ will include the following header in every request made to
 * _S_:
 *
 *     Cookie: Pants=On
 *
 * In SessionStore, cookies are abstracted, and they'll be created as needed by
 * some operations. If it finds a valid cookie, it'll use, but if not, it'll
 * create one.
 *
 * The "valid cookie" term depends on the implementation used.
 *
 * ### Attacks using session fixation ###
 *
 * These attacks exploits applications that allows one user fixate another's
 * user cookie. Consider the following code:
 *
 * \include sessionfixation.cpp
 *
 * Try to answer: What could happen with an application running the previous
 * code?
 *
 * Here is what can happens:
 *
 *   1. The attacker fixate the victim cookie's value to "I_KNOW_YOUR_ID".
 *      Depending on the application store's settings (including implementation
 *      and mac secret), an arbitrary value will be rejected by the store, but
 *      the attacker may still be able get an usable cookie value (maybe using
 *      the application itself to generate one).
 *   2. The victim access the application and perform the login.
 *   3. The application's store finds a cookie and reuses it to set the _user_
 *      property.
 *   4. The attacker uses the victim cookie's value and will have unlimited
 *      access to the victim's account.
 *
 * How an attacker could set the victim cookie value is out of the scope of this
 * document, because there are *several* techniques to achieve this.
 *
 * > Never trust users input.
 *
 * #### How defend against session fixation ####
 *
 * In Tufão, you can defend against session fixation calling
 * SessionStore::resetSession to force a new session to be created.
 *
 * \include sessionfixation2.cpp
 *
 * ### Attacks using cross site request forgery ###
 *
 * After the user is authenticated in your application, it can perform some
 * actions, such as transfer money to another account. If it's possible to
 * create a script to automate these actions, then it's possible for an
 * attacker to instruct the victim's user agent to perform this action. An
 * example of a scriptable action is the url below:
 *
 * > http://bank.example.com/withdraw?account=foo&amount=1000000&for=bar
 *
 * First, you *shouldn't* allow GET methods to mutate the server's state, but,
 * in this case, use POST wouldn't defend our users against attackers. To
 * prevent our actions from being scriptable, we need to require that an action
 * only will be valid if it's originated from our application.
 *
 * A technique to achieve the behaviour suggested in the previous paragraph is
 * to generate some random data (a challenge token) and associate it with the
 * user's current session. Then, we insert this token in the pages served to
 * this user. The following code shows part of the solution:
 *
 * \include csrf.cpp
 *
 * To generate a challenge token, you can use QUuid. To improve this design even
 * further, you can regenerate the token and name parameters for each request.
 *
 * Yet another option is to requiring the client to provide authentication data
 * in the same request used to perform the action, but this solution add a
 * usability issue.
 *
 * ### Other problems ###
 *
 * As defined in RFC 6265 and implemented in browsers, cookies don't provide
 * strong confidentiality. They don't provide:
 *
 *   - Isolation by port. A service running on one port has access to cookies
 *     of a service running on another port of the same host.
 *   - Isolation by scheme.
 *   - Full-isolation by path. User agents won't send cookies from one path to
 *     another, but it's still possible for a service to *set* a cookie of a
 *     service running in another path of the same host.
 *
 * Cookies don't provide strong integrity also.
 *
 * You can avoid the previous problems by signing or encrypting cookies, but
 * it'll still be possible for an attacker to perform a replay attack.
 *
 * You can signing cookies in Tufão using SessionStore::setMacSecret and hide
 * the session data using a SessionStore implementation that don't store its
 * data in the cookie.
 *
 * See SessionSettings for more information.
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
 * SessionSettings object and sign the cookies.
 *
 * This flexible design is what allows you, among other, implement a pure cookie
 * based storage mechanism.
 *
 * Security concerns
 * -----------------
 *
 * If the application request your store to manipulate some property and the
 * request provides a session's id that don't correspond to any session in the
 * store, you *should not* reuse this value as an id. You should *ignore it* and
 * create a new one. This design may add restrictions to some forms of attacks.
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
     * Destructs a SessionStore object.
     */
    ~SessionStore();

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
     * This method removes all cookies matching with this store's settings from
     * \p request.
     *
     * The purpose is hide the cookies from other pieces of code handling \p
     * request.
     *
     * Call this if you need to reset the session id.
     */
    void resetSession(HttpServerRequest &request) const;

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
     * Sets the secret key of the message authentication code. If set, it will
     * protects the cookies integrity and authenticity.
     *
     * \warning
     * The value used here *must* remain secret.
     *
     * The algorithm used by Tufão is HMAC + SHA1.
     */
    void setMacSecret(const QByteArray &secret);

    /*!
     * Returns the default settings, used when you don't specify the settings
     * argument in SessionStore constructor.
     *
     * The default values to these settings are:
     *   - timeout: 15
     *   - httpOnly: true
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
     *
     * \warning
     * If you get a session that doesn't exists, you *MUST NOT* reuse this id.
     * Create a new one and discard this, or you will introduce a session
     * fixation vulnerability.
     */
    QByteArray session(const HttpServerRequest &request) const;

    /*!
     * Returns the value of the first cookie that is compatible with this
     * store's properties, as defined in its settings.
     *
     * Besides searching in the \p request's headers named as "Cookie", search
     * in \p response's headers named as "Set-Cookie". This allows you to
     * manipulate sessions as soon as they are set.
     *
     * \warning
     * If you get a session that doesn't exists, you *MUST NOT* reuse this id.
     * Create a new one and discard this, or you will introduce a session
     * fixation vulnerability.
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

 private:
    QByteArray signSession(const QByteArray &message) const;
    QByteArray unsignSession(const QByteArray &message) const;

    struct Priv;
    Priv *priv;
};

} // Namespace Tufao

#endif // TUFAO_SESSIONSTORE_H
