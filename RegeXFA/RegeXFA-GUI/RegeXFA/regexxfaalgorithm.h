#ifndef REGEXXFAALGORITHM_H
#define REGEXXFAALGORITHM_H
#include <bits/stdc++.h>
using namespace std;
struct node{
    // 结点状态
    static const int simple=0,end=1<<2;
    map<char,set<node*> > to;// 出边
    int state;
    // node *fa; // 不确定会不会用到
    int num;// 结点编号
    set<int> st;// 代表的所有结点
    node(int n=0){
        num=n;
        st.insert(n);
        state=node::simple;
    }
};

class regexXfaAlgorithm
{
public:
    int tot,num;
    map<int,bool> vis;
    vector<set<int> >dfaState;
    vector<map<char,int> > jmp;
    vector<node*> nodes;
    vector<set<int> > minDfa;
    vector<int> uni;
    map<int,bool> minDfaIsEnd;// 最小化dfa第i结点是否终点
    int minDfaBg;// 最小化dfa起点的下标

    regexXfaAlgorithm();
    void init(const string &s);
    void destroyNodes();
    //左闭右开区间

    // Regex to NFA
    pair<node*,node*> build(const string &s,int bg,int ed);
    int dfsPrint(node *rt);

    // NFA to DFA
    void rmEps(node *fa,node *rt);
    void NFA2DFA_rmEps(node* rt);
    void getNodes(node *rt);
    void dfaPrint(int cur);

    // Minimize DFA
    void NFA2DFA(node *rt);
    void getMinDfa();
    void printMinDfa(node *bgNode);
    void getCode(string filename,const string &regexString);
};

#endif // REGEXXFAALGORITHM_H
