#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
class PluginDialog;
}

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginDialog(QWidget *parent = 0);
    ~PluginDialog();

    QVariant data() const;
    void setData(const QVariant &variant);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_chooseFilePushButton_clicked();
    void on_methodCheckBox_toggled(bool checked);

private:
    Ui::PluginDialog *ui;
};

#endif // PLUGINDIALOG_H
