/*  This file is part of the Tufão project
    Copyright (C) 2012, 2013 Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>

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

#include "tufaowizard.h"
#include "projecttypewizardpage.h"

#include <projectexplorer/customwizard/customwizard.h>

#include <QVariant>
#include <QDir>
#include <QFileInfo>
#include <QFile>

namespace Tufao {

TufaoWizardDialog::TufaoWizardDialog(QWidget *parent, const Core::WizardDialogParameters &parameters) :
    ProjectExplorer::BaseProjectWizardDialog(parent, parameters)
{
    setWindowTitle(trUtf8("New Tufão Web Server Project"));
    setIntroDescription(trUtf8("This wizard generates a"
                               " Tufão web server project."));
}

TufaoWizard::TufaoWizard(const Core::BaseFileWizardParameters &parameters,
                         QObject *parent) :
    Core::BaseFileWizard(parameters, parent)
{
}

QWizard *TufaoWizard::createWizardDialog(QWidget *parent,
                                         const Core::WizardDialogParameters &
                                         wizardDialogParameters) const
{
    TufaoWizardDialog *wizard
            = new TufaoWizardDialog(parent, wizardDialogParameters);
    setupWizard(wizard);

    wizard->addPage(new ProjectTypeWizardPage);

    {
        const WizardPageList &
                extensionPages(wizardDialogParameters.extensionPages());
        foreach (QWizardPage *p, extensionPages)
            BaseFileWizard::applyExtensionPageShortTitle(wizard,
                                                         wizard->addPage(p));
    }

    return wizard;
}

Core::GeneratedFiles TufaoWizard::generateFiles(const QWizard *w,
                                          QString *errorMessage) const
{
    Q_UNUSED(errorMessage)
    const TufaoWizardDialog *wizard = qobject_cast<const TufaoWizardDialog*>(w);

    QString projectType = wizard->field(QString::fromUtf8("type")).toString();
    QString projectName = wizard->projectName();
    QString projectPath = wizard->path() + QString::fromUtf8("/") + projectName
            + QString::fromUtf8("/");

    Core::GeneratedFiles ret;

    {
        QFile t(QString::fromUtf8(":/templates/metadata/") + projectType
                + QString::fromUtf8("/project.pro"));
        t.open(QIODevice::ReadOnly);
        QString content = QString::fromUtf8(t.readAll().constData())
                .arg(projectName);

        Core::GeneratedFile f(projectPath + projectName
                              + QString::fromUtf8(".pro"));
        f.setAttributes(Core::GeneratedFile::OpenProjectAttribute);
        f.setContents(content);

        ret.push_back(f);
    }

    QStringList files = QDir(QString::fromUtf8(":/templates/data/")
                             + projectType).entryList();
    for (QStringList::iterator i = files.begin();i != files.end();++i) {
        QFile t(QString::fromUtf8(":/templates/data/") + projectType
                + QString::fromUtf8("/") + *i);
        t.open(QIODevice::ReadOnly);

        Core::GeneratedFile f(projectPath + *i);
        f.setBinaryContents(t.readAll());

        if (*i != QString::fromUtf8("main.cpp"))
            f.setAttributes(Core::GeneratedFile::OpenEditorAttribute);

        ret.push_back(f);
    }

    return ret;
}

bool TufaoWizard::postGenerateFiles(const QWizard *,
                                    const Core::GeneratedFiles &l,
                                    QString *errorMessage)
{
    return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l,
                                                                  errorMessage);
}

} // namespace Tufao
