#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <Tufao/WebSocket>

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
    void onConnected();
    void onMessage(const QByteArray &msg);

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    Tufao::WebSocket websocket;
};

#endif // MAINWINDOW_H
