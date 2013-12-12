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
#include "classhandlermanager.h"

#include "classhandler.h"

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaType>
#include <QtPlugin>
#include <QPluginLoader>
#include <QSet>
#include <QString>
#include <QUrl>
#include <QVariant>

#include "httpserverrequest.h"

namespace Tufao {

// Initialize static members.
QStringList ClassHandlerManager::pluginLocations;

/* ****************************************************************************************************************** */
#pragma mark -
#pragma mark Object lifecycle
/* ****************************************************************************************************************** */
ClassHandlerManager::ClassHandlerManager(QString pluginID, QString context, QObject * parent) :
	QObject(parent),
	context(context),
	pluginID(pluginID)
{
	// Set up the search locations
	if(pluginLocations.isEmpty()) {
		// Add standard locations to pluginLocations
		// First, the typical app config dir's
#ifdef Q_OS_WIN
		// Code can be added here, but, for the time being, I'm leaving it empty.
#elif defined(Q_OS_MAC)
		pluginLocations.append(QDir::homePath() + "/Library/Application Support/Tufao");
		pluginLocations.append("/Library/Application Support/Tufao");
#else
		pluginLocations.append(QDir::homePath() + "/.tufao");
#endif

		// The standard library locations
		foreach (QString libraryPath, QCoreApplication::libraryPaths()) {
			QDir testDir(libraryPath + QDir::separator() + "Tufao");
			if(testDir.exists()){
				pluginLocations.append(testDir.absolutePath());
			}
		}

		// Finally, the install location
		QFileInfo installDir(QCoreApplication::applicationDirPath());
		if(installDir.isDir()) {
			pluginLocations.append(installDir.absolutePath());
		}

		qDebug() << "pluginLocations:" << pluginLocations;
	}

	// Now load the plugin of interest
	// First list all static plugins.
	foreach (QObject * pluginInterface, QPluginLoader::staticInstances()){
		ClassHandler * plugin = qobject_cast<ClassHandler *>(pluginInterface);
		if (plugin){
			registerHandler(plugin);
		}
	}
///
	// Then list dynamic libraries from the plugins/ directory
	QStringList contents;
#ifdef Q_OS_WIN
	QString pluginExtension = ".dll";
#else
#ifdef Q_OS_MAC
	QString pluginExtension = ".dylib";
#else
	QString pluginExtension = ".so";
#endif
#endif
	// retrieve a list of all dynamic libraries from the search paths
	foreach (QString path, pluginLocations) {
		QFileInfo thisPath(QDir(path).filePath("plugins"));
		if(thisPath.isDir()){
			QDir thisDir(thisPath.absoluteFilePath());
			qDebug() << "Search " << thisPath.absolutePath() << " for plugins.";
			foreach (const QString entry, thisDir.entryList()) {
				if (!(entry == "..") && !(entry == ".") && entry.endsWith(pluginExtension)){
					contents.append(thisDir.filePath(entry));
				}
			}
		}
	}

	// Check each dynamic library to see if it is a plugin
	foreach (QString pluginPath, contents) {
		QPluginLoader loader(pluginPath);
		// If we were constructed with a pluginID, we need to chech each plugin.
		if(pluginID.isEmpty() || pluginID == loader.metaData().value("IID").toString()) {
			if (!loader.load()) {
				qWarning() << "Couldn't load the dynamic library: "
							  << QDir::toNativeSeparators(pluginPath)
							  << ": "
							  << loader.errorString();
				continue;
			}

			QObject* obj = loader.instance();
			if (!obj) {
				qWarning() << "Couldn't open the dynamic library: "
							  << QDir::toNativeSeparators(pluginPath)
							  << ": "
							  << loader.errorString();
				continue;
			}

			ClassHandler * plugin = qobject_cast<ClassHandler *>(obj);
			if (plugin) {
				if (plugin){
					registerHandler(plugin);
				}
			}
		}

	}
}

ClassHandlerManager::~ClassHandlerManager()
{
	foreach (ClassHandlerManager::PluginDescriptor * descriptor, handlers) {
		delete descriptor;
	}
	handlers.clear();
}

/* ****************************************************************************************************************** */
#pragma mark -
#pragma mark Static Methods
/* ****************************************************************************************************************** */
void ClassHandlerManager::addPluginLocation(const QString location)
{
	if(!pluginLocations.contains(location)){
		pluginLocations.append(location);
	}
}

/* ****************************************************************************************************************** */
#pragma mark -
#pragma mark Private Methods
/* ****************************************************************************************************************** */
bool ClassHandlerManager::processRequest(HttpServerRequest & request,
													  HttpServerResponse & response,
													  const QString className,
													  const QString methodName,
													  const QHash<QString, QString> arguments)
{
	bool handled = false;
	bool canHandle = true;
	int methodIndex = selectMethod(className, methodName, arguments);
	if(methodIndex > -1) {
		ClassHandlerManager::PluginDescriptor * handler = handlers[className];
		QMetaMethod method = handler->handler->metaObject()->method(methodIndex);

		// Create the arguments
		QGenericArgument argumentTable[10];
		argumentTable[0] = Q_ARG(Tufao::HttpServerRequest, request);
		argumentTable[1] = Q_ARG(Tufao::HttpServerResponse, response);

		int argumentIndex = 2;
		while(argumentIndex < method.parameterCount()){
			QString parameterName = method.parameterNames()[argumentIndex];
			qDebug() << "Processing " << parameterName;
			QString argument = arguments.value(parameterName);
			QVariant variant = QVariant::fromValue(argument);
			int methodType = method.parameterType(argumentIndex);
			/* The if checks that follow are because I can not get the else (which was the original single implementation)
			 * to work for Qt data types. It seems to so far be working for the request & response.  More testing needed.
			 */
			if(methodType == QMetaType::Bool) {
				if (variant.canConvert(QMetaType::Bool)) {
					argumentTable[argumentIndex] = Q_ARG(bool, variant.toBool());
					qDebug() << "Processed bool " << variant.toBool() <<  "at index " << argumentIndex;
				} else {
					canHandle = false;
					qWarning() << "Can not convert " << argument << " to type bool";
				}
			} else if(methodType == QMetaType::Int) {
				if (variant.canConvert(QMetaType::Int)) {
					argumentTable[argumentIndex] = Q_ARG(int, variant.toInt());
					qDebug() << "Processed int " << variant.toInt() <<  "at index " << argumentIndex;
				} else {
					canHandle = false;
					qWarning() << "Can not convert " << argument << " to type int";
				}
			} else if(methodType == QMetaType::Double) {
				if (variant.canConvert(QMetaType::Double)) {
					argumentTable[argumentIndex] = Q_ARG(double, variant.toDouble());
					qDebug() << "Processed double " << variant.toDouble() <<  "at index " << argumentIndex;
				} else {
					canHandle = false;
					qWarning() << "Can not convert " << argument << " to type long";
				}
			} else if(methodType == QMetaType::QString) {
				if (variant.canConvert(QMetaType::QString)) {
					argumentTable[argumentIndex] = Q_ARG(QString, variant.toString());
					qDebug() << "Processed QString " << variant.toString() <<  "at index " << argumentIndex;
				} else {
					canHandle = false;
					qWarning() << "Can not convert " << argument << " to type QString";
				}
			} else {
				if(variant.canConvert(methodType)) {
					variant.convert(methodType);
					argumentTable[argumentIndex] = QGenericArgument(variant.typeName(), variant.data());
					qDebug() << "Converted " << argument << " to type " << QVariant::typeToName(methodType) << " index " <<argumentIndex;
				} else {
					canHandle = false;
					qWarning() << "Can not convert " << argument << " to type " << QVariant::typeToName(methodType);
				}
			}
			argumentIndex++;
		}
		if(canHandle) {
			method.invoke(handler->handler,
							  Qt::DirectConnection,
							  argumentTable[0],
							  argumentTable[1],
							  argumentTable[2],
							  argumentTable[3],
							  argumentTable[4],
							  argumentTable[5],
							  argumentTable[6],
							  argumentTable[7],
							  argumentTable[8],
							  argumentTable[9]
							  );
			handled = true;
		}
	} else {
		qWarning() << "Cound not find a method named with a matching signature.";
	}

	return handled;
}

void ClassHandlerManager::registerHandler(ClassHandler * handler)
{
	// Only process plugins that have not already been registered.
	if (!handlers.contains(handler->objectName())){
		 bool canDispathTo = false;
		 const QMetaObject* metaObject = handler->metaObject();
		 for(int methodIndex = metaObject->methodOffset(); methodIndex < metaObject->methodCount(); ++methodIndex) {
			 QMetaMethod method = metaObject->method(methodIndex);
			 // We only want public slots whos first two arguements are request & response
			 if(method.methodType() == QMetaMethod::Slot && method.access() == QMetaMethod::Public) {
				 QList<QByteArray> parameterNames = method.parameterNames();
				 if(parameterNames[0] == QByteArray("request") && parameterNames[1] == QByteArray("response")) {
					 canDispathTo = true;
					 ClassHandlerManager::PluginDescriptor * pluginDescriptor = handlers[handler->objectName()];
					 if(pluginDescriptor == NULL) {
						 pluginDescriptor = new ClassHandlerManager::PluginDescriptor();
						 handlers[handler->objectName()] = pluginDescriptor;
					 }
					 pluginDescriptor->className = handler->objectName();
					 pluginDescriptor->handler = handler;

					 uint parameterHash = qHash(QString::fromLatin1(method.name()));
					 foreach (QByteArray nameBytes, parameterNames) {
						 parameterHash += qHash(QString::fromLatin1(nameBytes));
					 }
					 pluginDescriptor->methods.insert(parameterHash, methodIndex);
					 pluginDescriptor->methodNames.append(QString::fromLatin1(method.name()));

					 QString signature = QString::fromLatin1(method.methodSignature());
				 }
			 }
		 }
		 if(canDispathTo) {
			 handler->init();
		 }
	}

}

int ClassHandlerManager::selectMethod(const QString className,
												  const QString methodName,
												  const QHash<QString, QString> arguments) const
{
	int methodIndex = -1;
	uint parameterHash = qHash(methodName);
	parameterHash += qHash(QString("request"));
	parameterHash += qHash(QString("response"));
	foreach (QString key, arguments.keys()) {
		parameterHash += qHash(key);
	}
	PluginDescriptor * pluginDescriptor = handlers[className];
	if (pluginDescriptor->methods.contains(parameterHash)) {
		methodIndex = pluginDescriptor->methods.value(parameterHash);
	}
	return methodIndex;
}

/* ****************************************************************************************************************** */
#pragma mark -
#pragma mark Override Tufao::AbstractHttpServerRequestHandler Methods
/* ****************************************************************************************************************** */
bool ClassHandlerManager::handleRequest(Tufao::HttpServerRequest & request, Tufao::HttpServerResponse & response)
{
	bool wasHandled = false;
	QStringList pathComponents = request.url().toString().split("/", QString::SkipEmptyParts);


	//Is the request for our context?
	bool useContext = !context.isEmpty();
	// There must be at least two path components (class & method), and 3 if a context is specified.
	int minimumPathComponents = useContext ? 3 : 2;
	if (pathComponents.length() < minimumPathComponents) {
		qWarning() << "Request was dispatched to handler, but too few path components found.  The path components are"
					  << pathComponents;
	} else if(pathComponents.length() > minimumPathComponents  + 16) {
		// We also can not have too many arguments; 16 is max, as that is 8 argumetns plus request & response
		qWarning() << "Request was dispatched to handler, but too many path components found.  The path components are"
					  << pathComponents;
	} else {
		if(!useContext || context == pathComponents[0]) {
			int pathIndex = useContext ? 1 : 0;
			QString className = pathComponents[pathIndex++];
			QString methodName = pathComponents[pathIndex++];
			// We need to have an even number of path components left
			if((pathComponents.length() - pathIndex) % 2 == 0) {
				// See if we have a class handler with a matching method
				if(handlers.contains(className) && handlers[className]->methodNames.contains(methodName)) {
					// Convert the remaining path components into an argument hash
					QHash<QString, QString> arguments;
					while(pathIndex < pathComponents.length()){
						arguments[pathComponents[pathIndex]] = pathComponents[pathIndex + 1];
						pathIndex += 2;
					}
					wasHandled = processRequest(request, response, className, methodName, arguments);
				}
			} else {
				qWarning() << "Can not dispath as an odd number of parameter components were supplied.";
			}

		}
	}

	return wasHandled;
}

} // namespace Tufao
