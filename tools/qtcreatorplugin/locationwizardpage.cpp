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

#include "locationwizardpage.h"
#include "ui_locationwizardpage.h"
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>

LocationWizardPage::LocationWizardPage(const QString &defaultPath,
                                       QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::LocationWizardPage)
{
    ui->setupUi(this);

    ui->pathLineEdit->setText(defaultPath);

    registerField("name", ui->nameLineEdit);
    registerField("location", ui->pathLineEdit);
}

LocationWizardPage::~LocationWizardPage()
{
    delete ui;
}

bool LocationWizardPage::isComplete() const
{
    QString name = field("name").toString();
    QString path = field("location").toString();
    QFileInfo pathInfo(path);

    if (name.isEmpty()) {
        ui->infoLabel->setText("<p style=\"color:#ff0000;\">"
                               "Project name is empty!</p>");
        return false;
    } else if (path.isEmpty()) {
        ui->infoLabel->setText("<p style=\"color:#ff0000;\">"
                               "Project path is empty!</p>");
        return false;
    } else if (!pathInfo.exists()) {
        ui->infoLabel->setText("<p style=\"color:#ff0000;\">"
                               + path + " doesn't exists.</p>");
        return false;
    } else if(!pathInfo.isDir()) {
        ui->infoLabel->setText("<p style=\"color:#ff0000;\">"
                               + pathInfo.canonicalFilePath()
                               + " isn't a directory.</p>");
        return false;
    } else if (QDir(path).exists(name)) {
        ui->infoLabel->setText("<p style=\"color:#ff0000;\">"
                               + name + " already exists.</p>");
        return false;
    }

    ui->infoLabel->clear();
    return true;
}

void LocationWizardPage::on_nameLineEdit_textChanged(const QString &)
{
    emit completeChanged();
}

void LocationWizardPage::on_pathLineEdit_textChanged(const QString &)
{
    emit completeChanged();
}

void LocationWizardPage::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this);
    if (dir.isEmpty())
        return;

    ui->pathLineEdit->setText(dir);
}
