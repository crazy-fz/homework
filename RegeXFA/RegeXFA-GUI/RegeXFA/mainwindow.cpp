#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    algr=new regexXfaAlgorithm();
}

MainWindow::~MainWindow()
{
    delete algr;
    delete ui;
}


void MainWindow::on_beginAnalysis_clicked()
{
    string s=ui->lineEdit->text().toStdString();
//    qDebug()<<QString::fromStdString(s);
    algr->init(s);
    QFile f("graph.md");
    qDebug()<<f.open(QIODevice::ReadOnly);
    QString qs=f.readAll();
//    qDebug()<<qs;
    ui->XfaBrowser->setText(qs);
    f.close();
    f.setFileName("outPutCode.cpp");
    f.open(QIODevice::ReadOnly);
    qs=f.readAll();
    ui->progBrowser->setText(qs);
    f.close();
}

void MainWindow::on_saveRegex_triggered()
{
    QString out=QFileDialog::getSaveFileName(this,tr("Save File"),"regex.txt",tr("text file"));
    if(out=="") return;

    QFile file(out);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::critical(this,"error","无法保存至\""+out+"\"");
        return ;
    }

    file.write(ui->lineEdit->text().toUtf8());
    file.close();
}

void MainWindow::on_openRegex_triggered()
{
    QString in=QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("text file(*)"));
    if(in=="") return;

    QFile file(in);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::critical(this,"error","无法打开\""+in+"\"");
        return ;
    }

    ui->lineEdit->setText(file.readAll());
    file.close();
}
