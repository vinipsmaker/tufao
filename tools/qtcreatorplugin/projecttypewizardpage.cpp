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

#include "projecttypewizardpage.h"
#include "ui_projecttypewizardpage.h"
#include <QFile>
#include <QDir>

ProjectTypeWizardPage::ProjectTypeWizardPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::ProjectTypeWizardPage)
{
    ui->setupUi(this);

    registerField("type", ui->comboBox, "currentText",
                  SIGNAL(currentIndexChanged(QString)));

    QStringList projects = QDir(":/templates/metadata").entryList();
    for (QStringList::iterator i = projects.begin();i != projects.end();++i) {
        QFile f(":/templates/metadata/" + *i + "/motd");
        f.open(QIODevice::ReadOnly);
        ui->comboBox->addItem(*i, QString::fromUtf8(f.readAll().constData()));
    }

    ui->label->setText(ui->comboBox->itemData(ui->comboBox->currentIndex())
                       .toString());
}

ProjectTypeWizardPage::~ProjectTypeWizardPage()
{
    delete ui;
}

void ProjectTypeWizardPage::on_comboBox_currentIndexChanged(int index)
{
    ui->label->setText(ui->comboBox->itemData(index).toString());
}
