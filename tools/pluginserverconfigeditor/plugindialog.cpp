#include "plugindialog.h"
#include "ui_plugindialog.h"
#include "datahandler.h"
#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>

PluginDialog::PluginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginDialog)
{
    ui->setupUi(this);
}

PluginDialog::~PluginDialog()
{
    delete ui;
}

QVariant PluginDialog::data() const
{
    DataHandler d;
    d.setFileName(ui->fileLineEdit->text());

    {
        QString pattern(ui->regexLineEdit->text());
        Qt::CaseSensitivity sensitivity;
        QRegExp::PatternSyntax syntax(QRegExp::RegExp);

        if (ui->caseSensitivyCheckBox->isChecked())
            sensitivity = Qt::CaseSensitive;
        else
            sensitivity = Qt::CaseInsensitive;

        {
            QString s = ui->syntaxComboBox->currentText();
            if (s == "RegExp")
                syntax = QRegExp::RegExp;
            else if (s == "RegExp2")
                syntax = QRegExp::RegExp2;
            else if (s == "Wildcard")
                syntax = QRegExp::Wildcard;
            else if (s == "WildcardUnix")
                syntax = QRegExp::WildcardUnix;
            else if (s == "FixedString")
                syntax = QRegExp::FixedString;
            else if (s == "W3CXmlSchema11")
                syntax = QRegExp::W3CXmlSchema11;
            else
                qWarning("Invalid value for syntax combo box");
        }

        QRegExp regex(pattern, sensitivity, syntax);

        if (!ui->greedyCheckBox->isChecked())
            regex.setMinimal(true);

        d.setUrlRegex(regex);
    }

    if (!ui->methodCheckBox->isChecked())
        d.setMethod(ui->methodLineEdit->text().toUtf8());

    return d;
}

void PluginDialog::setData(const QVariant &variant)
{
    DataHandler d(variant);
    ui->fileLineEdit->setText(d.fileName());

    {
        QRegExp regex(d.urlRegex());

        ui->regexLineEdit->setText(regex.pattern());

        if (regex.caseSensitivity() == Qt::CaseSensitive)
            ui->caseSensitivyCheckBox->setChecked(true);
        else
            ui->caseSensitivyCheckBox->setChecked(false);

        switch (regex.patternSyntax()) {
        case QRegExp::RegExp:
            ui->syntaxComboBox->setCurrentIndex(0);
            break;
        case QRegExp::RegExp2:
            ui->syntaxComboBox->setCurrentIndex(1);
            break;
        case QRegExp::Wildcard:
            ui->syntaxComboBox->setCurrentIndex(2);
            break;
        case QRegExp::WildcardUnix:
            ui->syntaxComboBox->setCurrentIndex(3);
            break;
        case QRegExp::FixedString:
            ui->syntaxComboBox->setCurrentIndex(4);
            break;
        case QRegExp::W3CXmlSchema11:
            ui->syntaxComboBox->setCurrentIndex(5);
            break;
        }

        if (regex.isMinimal())
            ui->greedyCheckBox->setChecked(false);
        else
            ui->greedyCheckBox->setChecked(true);
    }

    if (d.hasMethod()) {
        ui->methodLineEdit->setText(d.method());
        ui->methodCheckBox->setChecked(false);
    } else {
        ui->methodCheckBox->setChecked(true);
    }
}

void PluginDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PluginDialog::on_chooseFilePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);
    if (filename.isEmpty())
        return;

    ui->fileLineEdit->setText(QFileInfo(filename).absoluteFilePath());
}

void PluginDialog::on_methodCheckBox_toggled(bool checked)
{
    if (checked)
        ui->methodLineEdit->setEnabled(false);
    else
        ui->methodLineEdit->setEnabled(true);
}
