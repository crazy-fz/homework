#ifndef TRIE_H
#define TRIE_H
#include <string>
#include <utility>

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

    Trie();
    void del(node *cur);
    virtual ~Trie();

    void insert(int type,const std::string &s);

    /**
     * @param s    string
     *
     * @return    pair<int,int>{类型，最后匹配成功的下标}
     */
    std::pair<int,int> query(const std::string &s);

private:
    /**
     * @param type    插入的类型 0: 关键字，1: 操作符号
     * @param i    当前下标
     * @param len    string长度
     * @param cur    当前所处字典树结点
     */
    void insert(int type,const std::string &s,std::string::size_type i,std::string::size_type len,node *cur);

    /**
     * @param s    string
     * @param i    当前下标
     * @param len    string长度
     * @param cur    当前所处字典树结点
     *
     * @return    pair<int,int>{类型，最后匹配成功的下标}
     */
    std::pair<int,int> query(const std::string &s,std::string::size_type i,std::string::size_type len,node *cur);

};

#endif // TRIE_H
