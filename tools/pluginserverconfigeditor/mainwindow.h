#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_addPushButton_clicked();
    void on_deletePushButton_clicked();
    void on_moveUpPushButton_clicked();
    void on_moveDownPushButton_clicked();
    void on_tableWidget_cellActivated(int row, int);

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();

    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();

private:
    void save(const QString &filename);
    void load(const QString &filename);

    Ui::MainWindow *ui;
    QString currentFilename;
    bool saved;
};

#endif // MAINWINDOW_H
