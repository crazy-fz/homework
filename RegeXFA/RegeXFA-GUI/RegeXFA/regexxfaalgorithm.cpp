#include "regexxfaalgorithm.h"

regexXfaAlgorithm::regexXfaAlgorithm()
{
}
void regexXfaAlgorithm::init(const string &s){
    destroyNodes();
    tot=0;
    num=0;
    dfaState.clear();
    jmp.clear();
    minDfa.clear();
    uni.clear();
    minDfaIsEnd.clear();

    vis.clear();
    freopen("graph.md","w",stdout);
    cout<<"# NFA\n";
    cout<<"```mermaid\ngraph LR\n";
    auto a=build(s,0,s.length());
    a.second->state|=node::end;
    vis.clear();
    cout<<R"*+(0(begin)-->)*+"<<a.first->num<<'\n';
    dfsPrint(a.first);
    cout<<"\n```\n";


    cout<<"# DFA\n";
    cout<<"```mermaid\ngraph LR\n";
    cout<<tot<<R"*+((begin)-->0)*+"<<'\n';
    vis.clear();
    NFA2DFA(a.first);
    cout<<"\n```\n";


    getMinDfa();
    cout<<"# Minimized DFA\n";
    printMinDfa(a.first);

    getCode("outPutCode.cpp",s);
    fclose(stdout);
}

void regexXfaAlgorithm::destroyNodes(){
    for(auto n:nodes){
        delete n;
    }
    nodes.clear();
}
pair<node*,node*> regexXfaAlgorithm::build(const string &s,int bg,int ed){
    /**
     * @brief  建立表达式树，顺便把NFA造好
     * @return <当前子图的起点，终点>
     */
    if(bg+1==ed){
        // 递归出口
        node *retB=new node(++tot);
        node *retE=new node(++tot);
        retB->to[s[bg]].insert(retE);
        return {retB,retE};
    }

    vector<int> v;
    int lay=0;
    // 找出外层 |
    for(int i=bg;i<ed;++i){
        if(s[i]=='(') ++lay;
        else if(s[i]==')') --lay;
        else if(lay==0 && s[i]=='|') v.push_back(i);
    }
    if(!v.empty()){
        // | 连接
        int l=bg;
        node *retB=new node(++tot);
        node *retE=new node(++tot);
        for(auto i:v){
            auto son=build(s,l,i);
            retB->to['$'].insert(son.first);
            son.second->to['$'].insert(retE);
            l=i+1;
        }
        auto son=build(s,l,ed);
        retB->to['$'].insert(son.first);
        son.second->to['$'].insert(retE);
        return {retB,retE};
    }
    else{
        // & 连接
        bool hasp=0;
        lay=0;
        int nx=bg;
        for(;nx<ed;++nx){
            if(s[nx]=='(') ++lay,hasp=1;
            else if(s[nx]==')') --lay;
            if(lay==0) break;
        }

        node *retB=nullptr;
        node *retE=retB;
        if(hasp){
            auto son=build(s,bg+1,nx);
            retB=son.first;
            retE=son.second;
        }
        else{
            auto son=build(s,bg,nx+1);
            retB=son.first;
            retE=son.second;
        }
        if(nx<ed-1 && s[nx+1]=='*'){
            ++nx;
            retE->to['$'].insert(retB);
            node *tmp=new node(++tot);
            tmp->to['$'].insert(retB);
            retB=tmp;
            tmp=new node(++tot);
            retE->to['$'].insert(tmp);
            retE=tmp;
            retB->to['$'].insert(retE);
        }
        if(nx<ed-1){
            auto son=build(s,nx+1,ed);
            retE->to['$'].insert(son.first);
            retE=son.second;
        }
        return {retB,retE};
    }
}

int regexXfaAlgorithm::dfsPrint(node *rt){
    /**
     * @brief 递归输出NFA
     */
    // if(rt->to.empty()||vis[rt->num]||rt->fa!=rt) return vis[rt->num];
    if(rt->to.empty()||vis[rt->num]) return vis[rt->num];
    vis[rt->num]=++num;
    // cout<<vis[rt];
    for(auto E:rt->to){
        for(auto t:E.second){
            dfsPrint(t);
            cout<<rt->num;
            if(rt->state&node::end){
                cout<<"((";
                for(auto it:rt->st)
                    cout<<it<<',';
                // cout<<rt->num;
                cout<<"))";
            }
            else{
                cout<<"[";
                for(auto it:rt->st)
                    cout<<it<<',';
                // cout<<rt->num;
                cout<<"]";
            }
            cout<<"--"<<E.first<<"-->"<<t->num;
            if(t->state&node::end){
                cout<<"((";
                for(auto it:t->st)
                    cout<<it<<',';
                // cout<<t.second->num;
                cout<<"))";
            }
            else{
                cout<<"[";
                for(auto it:t->st)
                    cout<<it<<',';
                // cout<<t.second->num;
                cout<<"]";
            }
            cout<<'\n';
        }
    }
    return vis[rt->num];
}

void regexXfaAlgorithm::rmEps(node *fa,node *rt){
    /**
     * @brief  把fa可通过eps到达的非eps结点全部接上fa
     */
    // rt->fa=fa;
    fa->st.insert(rt->st.begin(),rt->st.end());
    if(rt->state&node::end)
        fa->state|=node::end;
    if(rt->to.empty()){
        return;
    }
    for(auto i:rt->to){
        if(i.first!='$')
            fa->to[i.first].insert(i.second.begin(),i.second.end());
        else{
            for(auto j:i.second)
                rmEps(fa,j);
        }
    }

}

void regexXfaAlgorithm::NFA2DFA_rmEps(node* rt){
    /**
     * @brief 将rt可达的eps全部直连，并把编号merge
     */
    queue<pair<char,node*> >q;
    for(auto t:rt->to){
        for(auto j:t.second){
            q.push({t.first,j});
        }
    }
    rt->to.clear();
    while(!q.empty()){
        auto cur=q.front();
        q.pop();
        if(cur.first!='$')
            rt->to[cur.first].insert(cur.second);
        else rmEps(rt,cur.second);
    }
}

void regexXfaAlgorithm::getNodes(node *rt){
    /**
     *  @brief 根据node::num建立nodes(int)->node*的映射
     */
    if(vis[rt->num]) return;
    vis[rt->num]=1;
    nodes[rt->num]=rt;
    for(auto i:rt->to)
        for(auto j:i.second)
            getNodes(j);
}

void regexXfaAlgorithm::dfaPrint(int cur){
    /**
     *  @brief 递归输出初步的dfa
     */
    if(vis[cur]) return ;
    vis[cur]=1;
    // cout<<vis[rt];
    for(auto E:jmp[cur]){
            int t=E.second;
            dfaPrint(t);
            cout<<cur;
            bool isEd=0;
            for(auto it:dfaState[cur]){
                if(nodes[it]->state&node::end){
                    isEd=1;
                    break;
                }
            }
            if(isEd){
                cout<<"((";
                for(auto it:dfaState[cur]){
                    cout<<it<<',';
                }
                minDfa[0].insert(cur);
                // cout<<rt->num;
                cout<<"))";
            }
            else
            {
                cout<<"[";
                for(auto it:dfaState[cur]){
                    cout<<it<<',';
                }
                minDfa[1].insert(cur);
                // cout<<rt->num;
                cout<<"]";
            }
            cout<<"--"<<E.first<<"-->"<<t;

            isEd=0;
            for(auto it:dfaState[t]){
                if(nodes[it]->state&node::end){
                    isEd=1;
                    break;
                }
            }
            if(isEd){
                cout<<"((";
                for(auto it:dfaState[t]){
                    cout<<it<<',';
                }
                minDfa[0].insert(t);
                // cout<<t.second->num;
                cout<<"))";
            }
            else
            {
                cout<<"[";
                for(auto it:dfaState[t]){
                    cout<<it<<',';
                }
                minDfa[1].insert(t);
                // cout<<t.second->num;
                cout<<"]";
            }
            cout<<'\n';
    }
    return ;
}

void regexXfaAlgorithm::NFA2DFA(node *rt){
    /**
     *  @brief NFA转DFA
     */
    nodes.resize(tot+10);
    jmp.clear();
    vis.clear();
    getNodes(rt);
    for(int i=1;i<=tot;++i){
        NFA2DFA_rmEps(nodes[i]);
    }
    dfaState.clear();
    dfaState.push_back(rt->st);
    jmp.push_back(map<char,int>());
    set<int> nxt;
    for(int cur=0;cur<dfaState.size();++cur){
        for(char ch='0';ch<='z';++ch){
            // to rm
            if(ch=='$') continue;
            nxt.clear();
            for(auto i:dfaState[cur]){
                for(auto E:nodes[i]->to){
                    if(E.first==ch){
                        for(auto j:E.second)
                            nxt.insert(j->st.begin(),j->st.end());
                    }
                }
            }
            if(nxt.empty()) continue;
            else{
                bool flag=1;
                for(int i=0;i<dfaState.size();++i){
                    if(dfaState[i]==nxt){
                        flag=0;
                        jmp[cur][ch]=i;
                        break;
                    }
                }
                if(flag) {
                    dfaState.push_back(nxt);
                    jmp.push_back(map<char,int>());
                    jmp[cur][ch]=dfaState.size()-1;
                }
                    // cout<<'\n';
            }
        }
    }
    // for(auto i:dfaState){
    //     for(auto j:i)
    //         cout<<j<<' ';
    //     cout<<'\n';
    // }
    vis.clear();
    minDfa.clear();
    minDfa.push_back(set<int>());
    minDfa.push_back(set<int>());
    dfaPrint(0);

}

void regexXfaAlgorithm::getMinDfa(){
    /**
     *  @brief 获取最小化dfa
     */
    uni.resize(tot+10);
    for(int i=0;i<minDfa.size();++i){
        if(minDfa[i].empty()){
            minDfa.erase(minDfa.begin()+i);
            --i;
        }
    }
    bool f=1;
    while(f){
        f=0;
        // for(auto i:minDfa){
        //     for(auto j:i)
        //         cout<<j<<' ';
        //     cout<<'\n';
        // }
        // cout<<"--------\n";
        for(int i=0;i<minDfa.size();++i){
            for(auto j:minDfa[i])
                uni[j]=i;
        }
        for(int i=0;i<minDfa.size();++i){
            set<int> s1,s2;
            auto &bg=*minDfa[i].begin();
            s1.insert(bg);
            for(auto j:minDfa[i]){
                if(jmp[j].size()!=jmp[bg].size())
                    s2.insert(j);
                else{
                    bool diff=0;
                    for(auto k:jmp[bg]){
                        auto it=jmp[j].find(k.first);
                        if(it==jmp[j].end()){
                            s2.insert(j);
                            diff=1;
                            break;
                        }
                        else if(uni[k.second]!=uni[it->second]){
                            s2.insert(j);
                            diff=1;
                            break;
                        }
                    }
                    if(diff==0)
                        s1.insert(j);
                }
            }
            if(!s2.empty()){
                minDfa[i]=s1;
                minDfa.push_back(s2);
                f=1;
                break;
            }
        }
    }
}

void regexXfaAlgorithm::printMinDfa(node *bgNode){
    /**
     *  @brief 输出最小化dfa
     */
    minDfaIsEnd.clear();
    cout<<"```mermaid\ngraph LR\n";
    for(int i=0;i<minDfa.size();++i){
        for(auto j:jmp[*minDfa[i].begin()]){
            for(auto k:minDfa[i]){
                for(auto g:dfaState[k]){
                    if(nodes[g]->state&node::end)
                        minDfaIsEnd[i]=1;
                    if(nodes[g]==bgNode)
                        minDfaBg=i;
                }
            }
            cout<<i;
            if(minDfaIsEnd[i]==1)
                cout<<"((";
            else cout<<"[";
            set<int> st;
            for(auto k:minDfa[i]){
                for(auto g:dfaState[k])
                    st.insert(g);
            }
            for(auto n:st)
                cout<<n<<',';

            if(minDfaIsEnd[i]==1)
                cout<<"))";
            else cout<<"]";
            cout<<"--"<<j.first<<"-->"<<uni[j.second];


            for(auto k:minDfa[uni[j.second]]){
                for(auto g:dfaState[k])
                    if(nodes[g]->state&node::end)
                        minDfaIsEnd[uni[j.second]]=1;
            }
            if(minDfaIsEnd[uni[j.second]]==1)
                cout<<"((";
            else cout<<"[";
            st.clear();
            for(auto k:minDfa[uni[j.second]]){
                for(auto g:dfaState[k])
                    st.insert(g);
            }
            for(auto n:st)
                cout<<n<<',';
            if(minDfaIsEnd[uni[j.second]]==1)
                cout<<"))";
            else cout<<"]";
            cout<<'\n';
        }
    }
    cout<<tot<<R"*+((begin)-->)*+"<<minDfaBg<<'\n';
    cout<<"\n```\n";
}

void regexXfaAlgorithm::getCode(string filename,const string &regexString){
    ofstream fout(filename);
    fout<<"// "<<regexString<<'\n';
    // raw string
    fout<<R"*+(#include<iostream>
#include<string>
using namespace std;
bool my_regex_match(const string &s){
    int state=)*+";
    fout<<minDfaBg;
    fout<<";\n    bool isEd=";
    if(minDfaIsEnd[minDfaBg])
        fout<<"true";
    else fout<<"false";

    // raw string
    fout<<R"*+(;
    int len=s.length();
    for(int i=0;i<len;++i){
        switch (state)
        {)*+";
    for(int n=0;n<minDfa.size();++n){
        fout<<"\n        case ";
        fout<<n;
        fout<<R"*+(:
            )*+";
        if(jmp[*minDfa[n].begin()].empty()){
            fout<<"return false;";
        }
        else{
            // has edge
            int t=0;
            for(auto i:jmp[*minDfa[n].begin()]){
                string if_or_else;
                string isWhat;
                string nxtState="state="+to_string(uni[i.second]);
                if(minDfaIsEnd[uni[i.second]])
                    nxtState+=",isEd=true";
                nxtState+=';';
                if(t==0){
                    if_or_else="if";
                }
                else if_or_else="else if";
                ++t;
                isWhat="(s[i]=='";
                isWhat+=i.first;
                isWhat+="')";
                // if(i.first=='D'){
                //     isWhat="(isdigit(s[i]))";
                // }
                // else if(i.first=='L'){
                //     isWhat="(isalpha(s[i]))";
                // }

                fout<<if_or_else+isWhat;
                fout<<R"*+(
                )*+";
                fout<<nxtState;
                fout<<R"*+(
            )*+";
            }
            fout<<"else return false;";
        }
        fout<<R"*+(
            break;)*+";
        fout<<'\n';
    }
    fout<<R"*+(
        default:
            return false;
            break;
        }
    }
    if(isEd)
        return true;
    else return false;
}
int main(){
    string s;
    cin>>s;
    cout<<my_regex_match(s);
    return 0;
})*+";
}
