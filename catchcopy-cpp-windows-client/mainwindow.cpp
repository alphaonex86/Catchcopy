#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

void MainWindow::on_pushButton_clicked()
{
        if(!client.connectToServer())
                QMessageBox::critical(this,"error","unable to connect");
}

void MainWindow::on_pushButton_2_clicked()
{
        client.disconnectFromServer();
}

void MainWindow::on_pushButton_3_clicked()
{
        if(!client.sendProtocol())
                QMessageBox::critical(this,"error","the query have failed");
}

void MainWindow::on_pushButton_4_clicked()
{
        if(!client.setClientName(ui->lineEdit->text().toStdWString()))
                QMessageBox::critical(this,"error","the query have failed");
}
