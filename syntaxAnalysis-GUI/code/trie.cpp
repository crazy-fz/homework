#include "trie.h"
Trie::Trie(){
    root=new node();
}
Trie::~Trie(){
    del(root);
    delete root;
}

void Trie::del(node *cur){
    /*
    for(int i=0;i<128;++i){
        if(cur->son[i]!=nullptr){
            del(cur->son[i]);
            delete cur->son[i];
        }
    }
*/

    for(auto &i:cur->son){
        del(i.second);
        delete i.second;
    }

}

void Trie::insert(int type,const std::string &s,std::string::size_type i,std::string::size_type len,node *cur){
    if(i==len){
        cur->type=type;
        return ;
    }
    if(cur->son[s[i]]==nullptr)
        cur->son[s[i]]=new node();
    insert(type,s,i+1,len,cur->son[s[i]]);
}

void Trie::insert(int type,const std::string &s){
    if(s.length()>0)
        insert(type,s,0,s.length(),root);
}

std::pair<int,int> Trie::query(const std::string &s,std::string::size_type i,std::string::size_type len,node *cur){
    auto it=cur->son.find(s[i]);
    if(it==cur->son.end() || i==len)
        return {cur->type,i-1};
    else return query(s,i+1,len,it->second);
}

std::pair<int,int> Trie::query(const std::string &s){
    return query(s,0,s.length(),root);
}
