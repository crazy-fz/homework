#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sol=new solution();
    defaultSavePath="";
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sol;
}

void MainWindow::resizeEvent(QResizeEvent *event){
    int width=this->geometry().width();
    int height=this->geometry().height();
    ui->codeText->setGeometry(int(width*0.05),int(height*0.05),int(width*0.4),int(height*0.85));
    ui->labelCode->setGeometry(ui->codeText->geometry().x(),ui->codeText->geometry().y()-20,54,12);
    ui->outputText->setGeometry(int(width*0.55),int(height*0.05),int(width*0.4),int(height*0.85));
    ui->labelOut->setGeometry(ui->outputText->geometry().x(),ui->codeText->geometry().y()-20,54,12);
}

void MainWindow::on_codeText_textChanged()
{
//    qDebug()<<ui->codeText->toPlainText();
    std::string s=ui->codeText->toPlainText().toStdString();
    s=sol->work(s);
    ui->outputText->setText(QString::fromStdString(s));
}

void MainWindow::on_loadFile_triggered()
{
    QString in=QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("c/cpp(*.cpp *.c)"));
    if(in=="") return;

    QFile file(in);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::critical(this,"error","无法打开\""+in+"\"");
        return ;
    }
    // 更新解析结果
    ui->codeText->setText(file.readAll());
    file.close();

    // 更新默认保存位置
    int i;
    for(i=in.length();i>=0;--i){
        if(in[i]=='/') break;
    }
    defaultSavePath=in.mid(0,i+1);
}

void MainWindow::on_saveFile_triggered()
{
    QString out=QFileDialog::getSaveFileName(this,tr("Save File"),defaultSavePath+"analysis.txt",tr("text file"));
    if(out=="") return;

    QFile file(out);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::critical(this,"error","无法保存至\""+out+"\"");
        return ;
    }

    file.write(ui->outputText->toPlainText().toUtf8());
    file.close();

    // 更新默认保存位置
    int i;
    for(i=out.length();i>=0;--i){
        if(out[i]=='/') break;
    }
    defaultSavePath=out.mid(0,i+1);
}

void MainWindow::on_conpressSave_triggered()
{

    QString out=QFileDialog::getSaveFileName(this,tr("Save File"),defaultSavePath+"compressedCode.txt",tr("text file"));
    if(out=="") return;

    QFile file(out);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::critical(this,"error","无法保存至\""+out+"\"");
        return ;
    }

    std::string s=ui->codeText->toPlainText().toStdString();
    s=sol->compressCode(s);
    file.write(QString::fromStdString(s).toUtf8());
    file.close();

    // 更新默认保存位置
    int i;
    for(i=out.length();i>=0;--i){
        if(out[i]=='/') break;
    }
    defaultSavePath=out.mid(0,i+1);
}
