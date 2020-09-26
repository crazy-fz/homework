#include "solution.h"
#include <fstream>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include <sstream>

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

std::string solution:: work(const std::string &s){
    std::stringstream fout;
    std::string::size_type cur=0;
    std::string::size_type len=s.length();

    while(cur<len){
        // 过滤空字符、换行符
        if(isspace(s[cur])){
            ++cur;
            continue;
        }
        // 特判.3e-5这种特殊浮点数
        if(s[cur]=='.'){
            if(cur+1<len && s[cur+1]<='9' && s[cur+1]>='0'){
                // 要么是数字组成部分，要么是在e后面的-号
                while(cur<len && ( isnum.find(s[cur])!=isnum.end()
                    || (s[cur]=='-' && (cur==0||s[cur-1]=='e'||s[cur-1]=='E')))) fout<<s[cur++];
                fout<<"\t数字\n";
                continue;
            }
        }
        if(s[cur]=='\"'){
            fout<<s[cur];
            while(cur<len){
                ++cur;
                fout<<s[cur];
                if(s[cur]=='\"' && s[cur-1]!='\\') break;
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
            else if((s[cur]>='0' && s[cur]<='9') || s[cur]=='-'){
                fout<<s[cur++];
                while(cur<len && ( isnum.find(s[cur])!=isnum.end()
                    || (s[cur]=='-' && (cur==0||s[cur-1]=='e'||s[cur-1]=='E')))) fout<<s[cur++];
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

            // 如果不是其他任意类型，就是标识符或无法识别的符号
            else {
                auto pre=cur;
                while(cur<len && (isalnum(s[cur]) || s[cur]=='_')) fout<<s[cur++];
                if(pre==cur){
                    fout<<s[cur]<<"\t 无法识别的错误语法！";//这里暂时先这样处理，后续可能增加定位提示
                    break;
                }
                fout<<"\t标识符\n";
            }
        }
    }

    return fout.str();
}

std::string solution::compressCode(const std::string &s){
    std::string::size_type len=s.length();
        std::string ret=" ";// 防止越界
        int flag=0;// 0正常，1在//里，2在/*里，3在"里，4在'里
        for(std::string::size_type i=0;i<len;++i){
            if(flag==0){
                if(s[i]=='\r') continue;// 拒绝正常情况的所有\r
                else if(s[i]=='#'){
                    ret+=s[i];
                    for(++i;i<len && s[i]!='\n';++i){
                        if(!isspace(s[i])) ret+=s[i];
                    }
                    if(i!=len-1)
                        ret+='\n';
                }
                else if(isspace(s[i])){
                    if(!isalnum(*(--ret.end())) ||(i!=len-1 && !isalnum(s[i+1])))
                        continue;
                    else ret+=s[i];
                }
                else if(s[i]=='/'){
                    if(i==len-1){
                        ret+=s[i];
                    }
                    else if(s[i+1]=='/'){
                        ++i;
                        flag=1;
                    }
                    else if(s[i+1]=='*'){
                        ++i;
                        flag=2;
                    }
                    else{
                        ret+=s[i];
                    }
                }
                else if(s[i]=='\"'){
                    ret+=s[i];
                    flag=3;
                }
                else if(s[i]=='\''){
                    ret+=s[i];
                    flag=4;
                }
                else ret+=s[i];
            }

            else if(flag==1){
                if(s[i]=='\n')
                    flag=0;
            }

            else if(flag==2){
                if(i<len-1 && s[i]=='*' && s[i+1]=='/'){
                    ++i;
                    flag=0;
                }
            }

            else if(flag==3){
                ret+=s[i];
                if(s[i]=='\"' && s[i-1]!='\\'){
                    flag=0;
                }
            }
            else if(flag==4){
                ret+=s[i];
                if(s[i]=='\'' && s[i-1]!='\\'){
                    flag=0;
                }
            }
        }
        return ret.substr(1,ret.length()-1);
}
