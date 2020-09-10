#include <bits/stdc++.h>
using namespace std;
int main(){
    /* string "¶ÎÂä×¢ÊÍ"
    */
    string str="abc de";
    char c='\n'; // char '·ûºÅ³£Á¿'
    scanf("%c",&c);
    int len=str.length();
    for(int i=0;i<len;++i)
        cout<<str[i];

    return 0;
}