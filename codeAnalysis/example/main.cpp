#include <bits/stdc++.h>
using namespace std;
// 判断 标识符，关键字，数（包括整数、浮点数），字符串、注释、特殊符号（分界符）和运算符号等。


string result[2]={"关键字","运算符号"};
set<char> sp;
set<char> isnum;

// 字典树，存关键字、运算符号
class Trie{
public:
    struct node{
        int type;
        node *son[128];
        node() {
            type=-1;
            for(int i=0;i<128;++i)
                son[i]=nullptr;
        }
    };
    node *root;

    Trie(){
        root=new node(); 
    }
    void del(node *cur){
        for(int i=0;i<128;++i){
            if(cur->son[i]!=nullptr)
                del(cur->son[i]),delete(cur->son[i]);
        }
    }
    virtual ~Trie(){
        del(root);
        delete root;
    }
    void insert(int type,string &s,int i,int len,node *cur){
        /**
         *  @parame type    插入的类型 0: 关键字，1: 操作符号
         * @parame i    当前下标
         * @parame len    string长度
         * @parame cur    当前所处字典树结点
         */
        if(i==len){
            cur->type=type;
            return ;
        }
        if(cur->son[s[i]]==nullptr)
            cur->son[s[i]]=new node();
        insert(type,s,i+1,len,cur->son[s[i]]);
    }
    void insert(int type,string &s){
        insert(type,s,0,s.length(),root);
    }


    pair<int,int> query(const string &s,int i,int len,node *cur){
        /**
         * @parame i    当前下标
         * @parame len    string长度
         * @parame cur    当前所处字典树结点
         * 
         * @return    {类型，最后匹配成功的下标}
         */
        if(cur->son[s[i]]==nullptr || i==len)
            return {cur->type,i-1};
        else return query(s,i+1,len,cur->son[s[i]]);
    }
    pair<int,int> query(const string &s){
        return query(s,0,s.length(),root);
    }
};

int work(char *inFileName,char *outFileName){
    Trie t;
    string s;
    ifstream fin;
    char c;
    // 导入各种记号
    fin.open("texts/keywords.txt",ios::in);
    if(fin.fail()){
        cerr<<"can't load keywords.txt";
        return 1;
    }
    while(fin>>s){
        t.insert(0,s);
    }
    fin.close();

    fin.open("texts/operators.txt",ios::in);
    if(fin.fail()){
        cerr<<"can't load operators.txt";
        return 1;
    }
    while(fin>>s){
        t.insert(1,s);
    }
    fin.close();

    fin.open("texts/specialCharacters.txt",ios::in);
    if(fin.fail()){
        cerr<<"can't load specialCharacters.txt";
        return 1;
    }
    while(fin>>s){
        sp.insert(s[0]);
    }
    fin.close();

    fin.open("texts/isnum.txt",ios::in);
    if(fin.fail()){
        cerr<<"can't load isnum.txt";
        return 1;
    }
    while(fin>>s){
        isnum.insert(s[0]);
        //-号后续再特判
    }
    fin.close();

    // 一次性读入整个文件
    fin.open(inFileName,ios::in);
    if(fin.fail()){
        cerr<<"文件打开失败\n";
        return 1;
    }
    ostringstream tmp;
    tmp<<fin.rdbuf();
    s=tmp.str();

    ofstream fout;
    fout.open(outFileName,ios::out);
    int cur=0;
    int len=s.length();

    // 匹配
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
            string tmp="";
            for(int i=0;cur+i<len;++i){
                if(!isspace(s[i+cur]))
                    tmp+=s[i+cur];
                else break;
            }
            auto ret=t.query(tmp);
            // 关键字或运算符号
            if(ret.first!=-1){
                fout<<tmp.substr(0,ret.second+1)<<"\t"<<result[ret.first]<<'\n';
                cur+=(ret.second+1);
            }
            // 数字
            else if(s[cur]>='0' && s[cur]<='9'){
                int i=0;
                while(cur<len && ( (s[cur]>='0' && s[cur]<='9') || isnum.find(s[cur])!=isnum.end() 
                    || (s[cur]=='-' && cur<s.length()-1 && s[cur+1]>='0' && s[cur+1]<='9'))) fout<<s[cur++];
                fout<<"\t数字\n";
            }
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

int main(int argc,char *argv[]){
    char inFile[1000],outFile[1000];
    // 如果没有指定文件
    if(argc==1){
        cout<<"输入要解析的程序文件目录: ";
        cin>>inFile;
    }
    else strcpy(inFile,argv[1]);

    // 从输入文件的位置决定输出文件目录
    strcpy(outFile,inFile);
    int len=strlen(outFile)-1;
    while(len>0 && outFile[len]!='\\' && outFile[len]!='/') --len;
    strcpy(outFile+len+1,"result.txt\0");

    if(work(inFile,outFile)==0)
        cout<<"解析结果已保存至"<<outFile<<'\n';
    else cerr<<"解析失败\n";

    return 0;
}