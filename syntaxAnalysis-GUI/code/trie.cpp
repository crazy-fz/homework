#include "trie.h"

Trie::Trie(){
    root=new node();
}
Trie::~Trie(){
    del(root);
    delete root;
}

void Trie::del(node *cur){
    for(int i=0;i<128;++i){
        if(cur->son[i]!=nullptr){
            del(cur->son[i]);
            delete(cur->son[i]);
        }
    }
}

void Trie::insert(int type,const std::string &s,std::string::size_type i,std::string::size_type len,node *cur){
    if(i==len){
        cur->type=type;
        return ;
    }
    if(cur->son[int(s[i])]==nullptr)
        cur->son[int(s[i])]=new node();
    insert(type,s,i+1,len,cur->son[int(s[i])]);
}

void Trie::insert(int type,const std::string &s){
    if(s.length()>0)
        insert(type,s,0,s.length(),root);
}

std::pair<int,int> Trie::query(const std::string &s,std::string::size_type i,std::string::size_type len,node *cur){
    if(cur->son[int(s[i])]==nullptr || i==len)
        return {cur->type,i-1};
    else return query(s,i+1,len,cur->son[int(s[i])]);
}

std::pair<int,int> Trie::query(const std::string &s){
    return query(s,0,s.length(),root);
}
