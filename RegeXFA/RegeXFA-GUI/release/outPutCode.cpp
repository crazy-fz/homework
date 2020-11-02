// a(a|b)*ab
#include<iostream>
#include<string>
using namespace std;
bool my_regex_match(const string &s){
    int state=1;
    bool isEd=false;
    int len=s.length();
    for(int i=0;i<len;++i){
        switch (state)
        {
        case 0:
            if(s[i]=='a')
                state=3;
            else if(s[i]=='b')
                state=2;
            else return false;
            break;

        case 1:
            if(s[i]=='a')
                state=2;
            else return false;
            break;

        case 2:
            if(s[i]=='a')
                state=3;
            else if(s[i]=='b')
                state=2;
            else return false;
            break;

        case 3:
            if(s[i]=='a')
                state=3;
            else if(s[i]=='b')
                state=0,isEd=true;
            else return false;
            break;

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
}