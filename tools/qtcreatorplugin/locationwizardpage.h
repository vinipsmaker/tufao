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

#ifndef LOCATIONWIZARDPAGE_H
#define LOCATIONWIZARDPAGE_H

#include <QWizardPage>

namespace Ui {
    class LocationWizardPage;
}

class LocationWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit LocationWizardPage(const QString &defaultPath,
                                QWidget *parent = 0);
    ~LocationWizardPage();

    bool isComplete() const;

private slots:
    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_pathLineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::LocationWizardPage *ui;
};

#endif // LOCATIONWIZARDPAGE_H
