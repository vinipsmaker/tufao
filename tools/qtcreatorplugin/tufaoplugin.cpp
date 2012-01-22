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

#include "tufaoplugin.h"
#include "tufaoconstants.h"
#include "tufaowizard.h"

#include <QtCore/QtPlugin>
#include <QIcon>

using namespace Tufao;

TufaoPlugin::TufaoPlugin()
{
}

TufaoPlugin::~TufaoPlugin()
{
}

bool TufaoPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    Core::BaseFileWizardParameters params(Core::IWizard::ProjectWizard);
    params.setIcon(QIcon(":/icon.png"));
    params.setDescription(QString
                          ::fromUtf8("Creates a Tufão web server project."));
    params.setDisplayName(QString::fromUtf8("Tufão Web Server"));
    params.setId(Tufao::Constants::TUFAO_WIZARD_ID);
    params.setCategory(Constants::TUFAO_WIZARD_CATEGORY);
    params.setDisplayCategory(trUtf8(Constants::TUFAO_WIZARD_TR_CATEGORY));

    addAutoReleasedObject(new TufaoWizard(params, this));
    
    return true;
}

void TufaoPlugin::extensionsInitialized()
{
}

ExtensionSystem::IPlugin::ShutdownFlag TufaoPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN2(Tufao, TufaoPlugin)
