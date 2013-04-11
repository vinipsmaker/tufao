#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork/QHostAddress>
#include <QtWidgets/QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    websocket.connectToHost(QHostAddress::LocalHost, 8080, "/chat");

    connect(&websocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&websocket, SIGNAL(newMessage(QByteArray)),
            this, SLOT(onMessage(QByteArray)));
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

void MainWindow::onConnected()
{
    ui->sendButton->setEnabled(true);
}

void MainWindow::onMessage(const QByteArray &msg)
{
    QLabel *label = new QLabel(msg, this);
    ui->chatLayout->addWidget(label);
}

void MainWindow::on_sendButton_clicked()
{
    websocket.sendMessage(ui->textLineEdit->text().toUtf8());
}
