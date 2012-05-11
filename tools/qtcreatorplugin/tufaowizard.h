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

#ifndef TUFAO_TUFAOWIZARD_H
#define TUFAO_TUFAOWIZARD_H

#include "tufao_global.h"
#include <coreplugin/basefilewizard.h>
#include <projectexplorer/baseprojectwizarddialog.h>

namespace Tufao {

class TUFAO_EXPORT TufaoWizardDialog:
        public ProjectExplorer::BaseProjectWizardDialog
{
    Q_OBJECT
public:
    explicit TufaoWizardDialog(QWidget *parent,
                               const Core::WizardDialogParameters &parameters);
};

class TUFAO_EXPORT TufaoWizard : public Core::BaseFileWizard
{
    Q_OBJECT
public:
    explicit TufaoWizard(const Core::BaseFileWizardParameters &parameters,
                         QObject *parent = 0);

protected:
    QWizard *createWizardDialog(QWidget *parent,
                                const Core::WizardDialogParameters &
                                wizardDialogParameters) const;
    Core::GeneratedFiles generateFiles(const QWizard *w,
                                       QString *errorMessage) const;
    bool postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l,
                           QString *errorMessage);
};

} // namespace Tufao

#endif // TUFAO_TUFAOWIZARD_H
