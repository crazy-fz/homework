#include "solution.h"
#include <fstream>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

solution::solution(){
    t=new Trie();
    loadQrc();
}
solution::~solution(){
    delete t;
}

int solution::loadQrc(){
    sp.clear();
    isnum.clear();
    std::ifstream fin;
    // 导入各种记号
    QFile file;
    file.setFileName(":/texts/keywords.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"can't load keywords.txt";
        return 1;
    }
    else{
        QTextStream fin(&file);
        QString qs;
        while(!fin.atEnd()){
            fin>>qs;
            t->insert(0,qs.toStdString());
        }
    }
    file.close();

    file.setFileName(":/texts/operators.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"can't load operators.txt";
        return 1;
    }
    else{
        QTextStream fin(&file);
        QString qs;
        while(!fin.atEnd()){
            fin>>qs;
            t->insert(1,qs.toStdString());
        }
    }
    file.close();

    file.setFileName(":/texts/specialCharacters.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"can't load specialCharacters.txt";
        return 1;
    }
    else{
        QTextStream fin(&file);
        QString qs;
        while(!fin.atEnd()){
            fin>>qs;
            sp.insert(qs.toStdString()[0]);
        }
    }
    file.close();

    file.setFileName(":/texts/isnum.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"can't load isnum.txt";
        return 1;
    }
    else{
        QTextStream fin(&file);
        QString qs;
        while(!fin.atEnd()){
            fin>>qs;
            isnum.insert(qs.toStdString()[0]);
            //-号后续再特判
        }
    }
    file.close();
    return 0;
}

int solution:: work(const QString &inFileName,const QString &outFileName){
    std::string s;
    QFile file;

    file.setFileName(inFileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"can't load"<<inFileName;
        return 1;
    }
    else{
        QTextStream fin(&file);
        QString qs=fin.readAll();
        // 这里虽然没有把中文转成gbk，但是中文编码是2倍英文编码长度，后续可以正常处理。
        s=qs.toStdString();
        /*转成中文
        QTextCodec *code=QTextCodec::codecForName("GBK");
        s=code->fromUnicode(qs).data();
        cout<<s<<endl;
        */
    }

    std::ofstream fout;
    QTextCodec *code=QTextCodec::codecForName("GBK"); // 支持写入到中文路径
    fout.open(code->fromUnicode(outFileName).data(),std::ios::out);
    std::string::size_type cur=0;
    std::string::size_type len=s.length();

    // 匹配，以utf8写入文件
    while(cur<len){
        // 过滤空字符、换行符
        if(isspace(s[cur])){
            ++cur;
            continue;
        }

        else if(s[cur]=='\"'){
            fout<<s[cur];
            while(cur<len){
                ++cur;
                fout<<s[cur];
                if(s[cur]=='\"') break;
            }
            fout<<"\t字符串常量\n";
            ++cur;
            continue;
        }

        else if(cur+1<len && s.substr(cur,2)=="//"){
            while(cur<len && s[cur]!='\n'){
                fout<<s[cur++];
            }
            fout<<"\t单行注释\n";
        }

        else if(cur+1<len && s.substr(cur,2)=="/*"){
            while(cur<len){
                fout<<s[cur];
                if(cur>0 && s[cur]=='/' && s[cur-1]=='*')
                    break;
                ++cur;
            }
            fout<<"\t段落注释\n";
            ++cur;
        }

        else if(sp.find(s[cur])!=sp.end()){
            fout<<s[cur]<<"\t特殊符号\n";
            ++cur;
        }

        else {
            // 以空白字符分界，读入一段字符，降低复杂度
            std::string tmp="";
            for(std::string::size_type i=0;cur+i<len;++i){
                if(!isspace(s[i+cur]))
                    tmp+=s[i+cur];
                else break;
            }

            auto ret=t->query(tmp);
            // 关键字或运算符号
            if(ret.first!=-1){
                fout<<tmp.substr(0,std::string::size_type(ret.second+1))<<"\t"<<result[ret.first]<<'\n';
                cur+=(std::string::size_type(ret.second+1));
            }

            // 数字
            else if(s[cur]>='0' && s[cur]<='9'){
                while(cur<len && ( (s[cur]>='0' && s[cur]<='9') || isnum.find(s[cur])!=isnum.end()
                    || (s[cur]=='-' && cur<s.length()-1 && s[cur+1]>='0' && s[cur+1]<='9'))) fout<<s[cur++];
                fout<<"\t数字\n";
            }

            // 符号常量
            else if(s[cur]=='\''){
                fout<<s[cur++];
                while(cur<len){
                    fout<<s[cur];
                    if(s[cur]=='\''){
                        ++cur;
                        break;
                    }
                    if(cur=='\\'){
                        fout<<s[cur+1];
                        cur+=2;
                    }
                    else ++cur;
                }
                fout<<"\t符号常量\n";
            }

            // 标识符
            else {
                while(cur<len && (isalnum(s[cur]) || s[cur]=='_')) fout<<s[cur++];
                fout<<"\t标识符\n";
            }
        }
    }

    fout.close();
    return 0;
}
