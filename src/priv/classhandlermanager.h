/*  This file is part of the Tufão project
    Copyright (C) 2013 Timothy Reaves treaves@silverfieldstech.com

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TUFAO_PRIV_CLASSHANDLERMANAGER_H
#define TUFAO_PRIV_CLASSHANDLERMANAGER_H

#include "../classhandlermanager.h"

namespace Tufao {

struct ClassHandlerManager::Priv
{
    Priv(const QString &pluginID, const QString &urlNamespace) :
        pluginID(pluginID),
        urlNamespace(urlNamespace)
    {}

    //! Maps a class name or pluginID to the PluginDescriptor for the plugin.
    QHash<QString, ClassHandlerManager::PluginDescriptor *> handlers;
    //! The IID of the plugins this manager will load.  May be empty.
    QString pluginID;
    //! The contect - first path component of the URI - this manager is responsible for.  May be empty.
    QString urlNamespace;
};

} // namespace Tufao

#endif // TUFAO_PRIV_CLASSHANDLERMANAGER_H
