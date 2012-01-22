/*  This file is part of the Tufão project
    Copyright (C) 2012 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#ifndef TUFAO_H
#define TUFAO_H

#include "tufao_global.h"

#include <extensionsystem/iplugin.h>

namespace Tufao {

class TUFAO_EXPORT TufaoPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    
public:
    TufaoPlugin();
    ~TufaoPlugin();
    
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();
};

} // namespace Tufao

#endif // TUFAO_H

