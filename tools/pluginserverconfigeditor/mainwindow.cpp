#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plugindialog.h"
#include "datahandler.h"

#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    saved(true)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_addPushButton_clicked()
{
    PluginDialog dialog;
    if (dialog.exec() != QDialog::Accepted)
        return;

    DataHandler d(dialog.data());
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    {
        QTableWidgetItem *item = new QTableWidgetItem(d.fileName());
        item->setData(Qt::UserRole, d);
        ui->tableWidget->setItem(row, 0, item);
    }
    ui->tableWidget->setItem(row, 1,
                             new QTableWidgetItem(d.urlRegex().pattern()));
    ui->tableWidget->setItem(row, 2,
                             new QTableWidgetItem(QString(d.method())));

    if (saved) {
        setWindowTitle(windowTitle() + "*");
        saved = false;
    }
}

void MainWindow::on_deletePushButton_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row == -1)
        return;

    ui->tableWidget->removeRow(row);

    if (saved) {
        setWindowTitle(windowTitle() + "*");
        saved = false;
    }
}

void MainWindow::on_moveUpPushButton_clicked()
{
    const int row = ui->tableWidget->currentRow();
    if (row == -1 || row == 0)
        return;

    QTableWidgetItem *items[3] = {ui->tableWidget->takeItem(row, 0),
                                  ui->tableWidget->takeItem(row, 1),
                                  ui->tableWidget->takeItem(row, 2)};

    for (int i = 0;i != 3;++i) {
        ui->tableWidget->setItem(row, i, ui->tableWidget->takeItem(row - 1, i));
        ui->tableWidget->setItem(row - 1, i, items[i]);
    }

    ui->tableWidget->selectRow(row - 1);
}

void MainWindow::on_moveDownPushButton_clicked()
{
    const int row = ui->tableWidget->currentRow();
    if (row == -1 || row + 1 == ui->tableWidget->rowCount())
        return;

    QTableWidgetItem *items[3] = {ui->tableWidget->takeItem(row, 0),
                                  ui->tableWidget->takeItem(row, 1),
                                  ui->tableWidget->takeItem(row, 2)};

    for (int i = 0;i != 3;++i) {
        ui->tableWidget->setItem(row, i, ui->tableWidget->takeItem(row + 1, i));
        ui->tableWidget->setItem(row + 1, i, items[i]);
    }

    ui->tableWidget->selectRow(row + 1);
}

void MainWindow::on_tableWidget_cellActivated(int row, int)
{
    QTableWidgetItem *item = ui->tableWidget->item(row, 0);
    PluginDialog dialog;
    dialog.setData(item->data(Qt::UserRole));

    if (dialog.exec() != QDialog::Accepted)
        return;

    DataHandler d(dialog.data());
    item->setData(Qt::UserRole, d);

    item->setText(d.fileName());
    ui->tableWidget->item(row, 1)->setText(d.urlRegex().pattern());
    ui->tableWidget->item(row, 2)->setText(QString(d.method()));

    if (saved) {
        setWindowTitle(windowTitle() + "*");
        saved = false;
    }
}

void MainWindow::on_actionNew_triggered()
{
    QMessageBox::StandardButton choice
            = QMessageBox::question(this, windowTitle(), "Are you sure?",
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes);
    if (choice != QMessageBox::Yes)
        return;

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    currentFilename.clear();
    setWindowTitle("New file");

    saved = false;
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this);
    if (filename.isEmpty())
        return;

    load(filename);
    setWindowTitle(filename);
}

void MainWindow::on_actionSave_triggered()
{
    if (currentFilename.isEmpty()) {
        on_actionSave_as_triggered();
        return;
    }

    save(currentFilename);
    setWindowTitle(currentFilename);
}

void MainWindow::on_actionSave_as_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this);
    if (filename.isEmpty())
        return;

    save(filename);
    setWindowTitle(filename);
}

void MainWindow::save(const QString &filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    settings.clear();

    settings.setValue("version", QString("0.1"));

    settings.beginWriteArray("plugins");
    for (int i = 0;i != ui->tableWidget->rowCount();++i) {
        settings.setArrayIndex(i);
        DataHandler d(ui->tableWidget->item(i, 0)->data(Qt::UserRole));
        settings.setValue("filename", d.fileName());
        settings.setValue("regex", d.urlRegex());
        if (d.hasMethod())
            settings.setValue("method", d.method());
    }
    settings.endArray();

    currentFilename = filename;
    saved = true;
}

void MainWindow::load(const QString &filename)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    QSettings settings(filename, QSettings::IniFormat);

    int size = settings.beginReadArray("plugins");
    for (int i = 0;i != size;++i) {
        settings.setArrayIndex(i);
        DataHandler d;
        d.setFileName(settings.value("filename").toString());
        d.setMethod(settings.value("method").toByteArray());
        d.setUrlRegex(settings.value("regex").toRegExp());
        ui->tableWidget->insertRow(i);
        {
            QTableWidgetItem *item = new QTableWidgetItem(d.fileName());
            item->setData(Qt::UserRole, d);
            ui->tableWidget->setItem(i, 0, item);
        }
        ui->tableWidget->setItem(i, 1,
                                 new QTableWidgetItem(d.urlRegex().pattern()));
        ui->tableWidget->setItem(i, 2,
                                 new QTableWidgetItem(QString(d.method())));
    }

    currentFilename = filename;
    saved = true;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, trUtf8("Tufão plugin server config editor"),
                       trUtf8("This application creates and edits configuration"
                              " files used by the Tufão Plugin Server class."));
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}
