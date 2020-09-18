#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sol=new solution();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sol;
}


void MainWindow::on_pushButton_clicked()
{
    QString in=QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("c/cpp(*.cpp *.c)"));

    // 默认输出位置设置在输入文件同目录下
    int i;
    for(i=in.length();i>=0;--i){
        if(in[i]=='/') break;
    }
    QString outfile=in.mid(0,i+1);
    outfile+="result.txt";

    QString out=QFileDialog::getSaveFileName(this,tr("Save File"),outfile,tr("text file"));
    if(out=="")
        return ;

    qDebug()<<in<<' '<<out<<'\n';
    int flag=sol->work(in,out);
    if(flag==0)
        QMessageBox::information(this,"解析成功","解析文件已保存至"+out);

    else
        QMessageBox::critical(this,"error","解析失败");

}
