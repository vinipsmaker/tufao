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

#include <QtPlugin>

namespace Tufao {

//! @struct ClassHandlerPluginInfo
//! Contains information about a ClassHandler plugin.
struct ClassHandlerPluginInfo
{
	ClassHandlerPluginInfo(){;}
	//! The plugin ID. It MUST match the lib file name (case sensitive).
	QString id;
	//! The displayed name, e.g. "ServerSettings".
	QString displayedName;
	//! The comma separated list of authors, e.g. "Timothy Reaves".
	QString authors;
	//! The contact email or URL.
	QString	contact;
	//! The HTML description of the plugin.
	QString description;
};


/*!
 * \brief The ClassHandler class
 * Define the interface to implement when creating a plugin.
 * The interface is used by the <a href="http://qt-project.org/doc/qt-5.0/qtcore/qpluginloader.html">QPluginLoader</a>
 * to load plugins dynamically.
 * @sa @ref plugins for documentation on how to develop external plugins.
 */
class ClassHandler : public QObject
{
	Q_OBJECT
public:
	virtual ~ClassHandler(){}

	virtual void deinit() = 0;
	virtual void init() = 0;

	//! Get information about the plugin.
	virtual ClassHandlerPluginInfo getPluginInfo() const = 0;

};

} // namespace Tufao

Q_DECLARE_INTERFACE(Tufao::ClassHandler, "tufao.ClassHandler/1.0")

#endif // TUFAO_CLASSHANDLER_H
