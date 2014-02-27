/*
  Copyright (c) 2013 Timothy Reaves treaves@silverfieldstech.com

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

#ifndef TUFAO_CLASSHANDLER_H
#define TUFAO_CLASSHANDLER_H

#include <QtCore/QtPlugin>

#define TUFAO_CLASSHANDLER_IID "Tufao::ClassHandler/1.0"

namespace Tufao {

//! @struct ClassHandlerPluginInfo
//! Contains information about a ClassHandler plugin.
struct ClassHandlerPluginInfo
{
    //! The plugin ID. It MUST match the lib file name (case sensitive).
    QString id;
    //! The displayed name, e.g. "ServerSettings".
    QString displayedName;
    //! The comma separated list of authors, e.g. "Timothy Reaves".
    QString authors;
    //! The contact email or URL.
    QString contact;
    //! The HTML description of the plugin.
    QString description;
};


/*!
 * \brief The ClassHandler class
 * Define the interface to implement when creating a plugin.
 *
 * <p>This interface is used by the <a href="http://qt-project.org/doc/qt-5.0/qtcore/qpluginloader.html">QPluginLoader
 * </a> to load plugins dynamically.</p>
 *
 * <p>This class is intended to be used in a very specific way.  If it is used in that way, and linked into a library,
 * then Tufao can automatically dispatch incoming requests to this class, and more specifically, to specific methods
 * defined in this class.  This allows the developer to just write a class, and not need to worry about crazy regular
 * expressions, and then logic to determine what should be done (action to perform) with the traditional
 * AbstractHttpServerRequestHandler.  As the AbstractHttpServerRequestHandler can only have one dispatch-able method, it
 * would have been up to the developer to determine what logic to execute based on the request, or to have a large
 * number of classes & a large number of regular expressions to map to those classes.</p>
 *
 * <p>This is achieved by breaking the URL paths up into a parameter list, and then invoking a method on the subclass
 * and passing the parameters to that method. So a URL request like:
 *      <tt>/forum/read/forumName/someForumName/threadName/someThreadName/page/6</tt>
 * would call a method Forum::read(QString forumName, QString threadName, int pageNumber) as
 * read("someForumName", "someThreadName", 6);. (please read below for a more accurate description).  This would take a
 * user to a forum named <em>someForumName</em>, to a topic named <em>someThreadName</em>, to the <em>6</em>th page of
 * responses.</p>
 *
 * <p>This format is intended to follow the general ReST tenants of resource location, and map them to a method
 * implemented on a ClassHandler.  In this example, some resource named <em>forum</em> has a sub-resource named
 * <em>forumName</em>, and that sub-resource further has a sub-resource named <em>threadName</em>, and finally, a
 * further sub-resource of <em>page</em>.  Each of the first two resources are found by a string, and the last resource
 * is found by an int.</p>
 *
 * <p>To use this class, there are a few conventions you must follow, and the class needs to be compiled into a plugin.
 * First I'll talk about the implementation, and then I'll talk about the packaging.<p>
 *
 * <h2>Implementation</h2>
 *
 * <h3>Creating a Class to match a path</h3>
 *
 * <p>All subclasses must be <em>Q_OBJECT</em>'s. Further, two additional macro's must be used:
 * <em>Q_PLUGIN_METADATA(IID "tufao.Test/1.0")</em> and <em>Q_INTERFACES(Tufao::ClassHandler)</em>.  The second macro
 * must be exact, and is what Qt uses to know the class implements the ClassHandler interface.  The first macro defines
 * metadata about the plugin, specifically the IID of the plugin.  The string can be anything meaningful, but is
 * typically the now-common reverse resource locator used by so many languages.  So if you were with the Stellarium
 * project, the IID would be something like <em>org.stellarium.PluginName/1.0</em> where <em>PluginName</em> would be
 * name of this specific plugin, and <em>1.0</em> would be the plugin version.</p>
 *
 * <p>In the URL, the first path component is the object name (technically, if a context is being used, the context will
 * be the first path component - see ClassHandlerManager - but that is transparent to implementing this class). In your
 * constructor, you should call <tt>setObjectName("forum");</tt> where the parameter is what you want matched in the URL
 * path.  For clarity, using the name of the class itself is a good idea.  As it is case-sensitive, you may want to use
 * the lower case version. Once this is done, incoming requests that match the object name will be dispatched (if they
 * match a method; see below).<p>
 *
 * <h3>Implementing methods as end-points</h3>
 *
 * <p>The next path component is the name of a method in the class.  The method name must match exactly, and is case-
 * sensitive.  The method should be a <tt>public slot</tt>, and the signature of the method is very important.  The
 * method must accept at least two parameters, and they must be <tt>Tufao::HttpServerRequest & request,
 * Tufao::HttpServerResponse & response</tt>.  The parameter names must be that exactly, as the dispatch uses parameter
 * names of the methods to find the correct method to dispatch to. Further, the parameter types are used to convert the
 * strings of the URL path to the data types the method requires.</p>
 *
 * <p>As these two parameters must be on every method, the simplest method could have would have these two parameters.
 * This allows you access to the request if you need it, and the response to write out the information back to the
 * requester.</p>
 *
 * <p>In the above example, the actual declaration of the method would be:
 * <tt>void read(Tufao::HttpServerRequest & request,
 *           Tufao::HttpServerResponse & response,
 *           QString forumName,
 *           QString threadName,
 *           int pageNumber)</tt>.  The parameter names are used to match the path components in the URL.  This means
 * that a URL will always have an <em>even number</em> of path components after the class name; half for the name of
 * the parameters, and the other half for the values.  If a value cannot be converted to the type of the parameter,
 * the request is not handled, and a message logged.  So the request would end up passing to the next registered
 * AbstractHttpServerRequestHandler registered with the HttpRequestRouter.</p>
 *
 * <p>If a context has been assigned to the ClassHandlerManager that is responsible for this plugin, it is also
 * important to check to see if the request has a context.  This is done simply with
 * <tt>!request.context().isEmpty()</tt>.  If the context is not empty, yo may need to set it into the output, so that
 * any URL's refernced (either in HTML or in Ajax) can prepend the context to there paths.  If you are the sole user of
 * your plugin, and you know what context you are going to use, you can avoid this and directly use the context in your
 * HTML.</p>
 *
 * <p>Finally, as the user has no control over instance lifetime (creation is done at application start for static
 * plugins and load-time for dynamic ones, and objects are destroyed at application termination for static plugins and
 * when the plugin is unloaded for dynamic ones), there are <tt>void init()</tt> and <tt>void deinit()</tt> methods
 * that are called before an instance is dispatched to, and before shutdown.  This allows for the creation or obtaining
 * of resources that should not be attempted until the QCoreApplication is running, and that should be freed before the
 * QCoreApplication is terminated.</p>
 *
 * <h2>Packaging</h2>
 *
 * <p>This class is also the superclass for plugins.  One you subclass, you compile that code into either a dynamic or a
 * static plugin.  Dynamic plugins are a bit more robust in this case, and you can find an example project in the
 * examples directory, named sample_plugin, that has a project with an executable as well as a dynamic plugin.  You can
 * use the cmake files for your own project.</p>
 *
 * <p>If using a static plugins, you can load as many plugins as you like, but, they will all be connected to the
 * same context (if used; see ClassHandlerManager).  This is because there is not way to determine what the IID of the
 * plugin is when loaded statically.</p>
 *
 * <p>If using a dynamic plugin, the plugin must be installed into a path where it will be found.  This includes all of
 * the following:
 * <ul>
 * <li>Every entry in the OS's library path with a <em>Tofao</em> sub-directory</li>
 * <li><em>/Library/Application Support/Tufao</em> on Mac.
 * <li><em>~/Library/Application Support/Tufao</em> on Mac.
 * <li><em>~/.tufao</em> on UNIX (non-Mac).</li>
 * <li>The CWD of the application itself</li>
 * </ul>
 * Each of these paths is searched for a plugins subdirectory.  Each entry in that directory that is a dynamic library
 * is loaded if it implements the ClassHandler interface.
 * </p>
 *
 *
 * \include ClassHandler
 * \since 1.2
 */


class ClassHandler : public QObject
{
    Q_OBJECT
public:
    virtual ~ClassHandler(){}

    //! Clean up resource; called automatically before plugin is unloaded.
    virtual void deinit() = 0;
    //! Initialize an instance for use; alled automatically when plugin is registered.
    virtual void init() = 0;

    //! Get information about the plugin.
    virtual ClassHandlerPluginInfo getPluginInfo() const = 0;

    static const QString HttpResponseStatusKey;
    static const QString JsonResponseKey;
};

} // namespace Tufao

Q_DECLARE_INTERFACE(Tufao::ClassHandler, TUFAO_CLASSHANDLER_IID)

#endif // TUFAO_CLASSHANDLER_H
