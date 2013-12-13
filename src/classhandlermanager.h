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

#ifndef TUFAO_CLASSHANDLERMANAGER_H
#define TUFAO_CLASSHANDLERMANAGER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QMap>
#include <QMultiHash>
#include <QStringList>

#include "abstracthttpserverrequesthandler.h"

namespace Tufao {

class ClassHandler;

/*!
 * \brief This class serves as the HttpServerRequestHandler for all ClassHandler plugins.
 *
 * <p>This class is used to mange one or more ClassHandler plugins.  To use this class, you simply instantiate and
 * register with the HttpServerRequestRouter.  You can specify that an instane of this class is only to manager
 * plugins with a particular IID, and you can also assign a context (the first path component of a URL).</p>
 *
 * <p>If you specify a context at creation time, only URL's with that string as the first path component
 * (case-sensitive) will recive dispatches.</p>
 *
 * <p>Static plugins will be registered be the first instance of this class created, regardless of if they specify an
 * IID (as IID is not available at runtime for static plugins).  Otehrwise, only plugins that have a matching IID
 * to the plginID provided will be loaded by this instance.</p>
 *
 * <p>To have a single instance load and regiser all plugins, simply leave the pluginID blank.<p>
 *
 * <p>If context is used, in your ClassHandler instances you will likely need to pass the value of the context out into
 * your response so that things like URL's in HTML or Ajaz calls can prepend the context to their paths; otherwise
 * the calls will fail.</p>
 *
 * \include ClassHandlerManager
 * \since 1.1
 */
class ClassHandlerManager : public QObject, public AbstractHttpServerRequestHandler
{
	Q_OBJECT
	Q_PROPERTY(QString context READ context)
public:
	/*!
	 * \brief Constructs a ClassHandlerManager object.
	 *
	 * \param pluginID if provided, this instance will only load plugins whose IID matches.
	 * \param context if provided, this is the context (first path component of the URI) for the plugins managed by this
	 * instance.  Before dispatching a request to one of the plugins, the cotext is checked, and only if it matches does
	 * the request get dispatched.
	 * \param parent is passed to the QObject constructor.
	 */
	explicit ClassHandlerManager(QString pluginID = "", QString context="", QObject * parent = 0);

	/*!
	  Destroys the object.
	  */
	virtual ~ClassHandlerManager();

	QString context(void) const;

	/*!
	 * \brief Adds a non-standard path to the search paths.
	 * By default, the standard locations are searched for plugins.  The standard paths are the system
	 * library paths with a Tufao sub-directory, the executable path, and the applications configutation directory; on
	 * linux, ~/.tufao, and on Mac both /Library/Application Support/Tufao and ~/Library/Application Support/Tufao.  Each
	 * of these directories is search to see if it has a plugins sud-directoy, and if it does, all dynamic libraries in
	 * each matching directroy is examined to determine if it is a plugin.
	 * \param location an absolute path to be added to the search paths for plugins.
	 */
	static void addPluginLocation(const QString location);

public slots:
	 bool handleRequest(HttpServerRequest & request, HttpServerResponse & response) override;

private:
	struct PluginDescriptor
	{
		friend class ClassHandlerManager;
		PluginDescriptor(){;}
		//! the refernece to the handler itself.
		ClassHandler * handler;
		//! The name of the object this handler is for.
		QString className;
		//! Maps the hash of the parameter names and method name, to the method index in the ClassHandler.
		QHash<uint, int> methods;
		//! A list of all of the method names the plugin can dispatch to.
		QList<QString> methodNames;
	};

	/*!
	 * \brief register a handler.
	 * This method is responsible for finding all of the public slots in the handler, and creating a PluginDescriptor
	 * to add to the handlers hash.
	 * \param handler the handler to register.
	 */
	void registerHandler(ClassHandler * handler);

	bool processRequest(HttpServerRequest & request,
							  HttpServerResponse & response,
							  const QString className,
							  const QString methodName,
							  const QHash<QString, QString> arguments);

	int selectMethod(const QString className, const QString methodName, const QHash<QString, QString> arguments) const;

	//! Maps a class name or pluginID to the PluginDescriptor for the plugin.
	QHash<QString, ClassHandlerManager::PluginDescriptor *> handlers;
	//! The IID of the plugins this manager will load.  May be empty.
	QString pluginID;
	//! The paths dearched to find plugins.
	static QStringList pluginLocations;

	//! The contect - first path component of the URI - this manager is responsible for.  May be empty.
	QString m_context;
};

} // namespace Tufao

#endif // TUFAO_CLASSHANDLERMANAGER_H
