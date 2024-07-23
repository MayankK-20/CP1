/**
 *    author:  MK_20
 *    created: 2024-07-21 21:01:01
 **/
//Question URL: https://codeforces.com/group/8bhJZQiaub/contest/537565/problem/A
//Rating:

#include<bits/stdc++.h>
using namespace std;

#include <bits/stdc++.h>
using namespace std;

const int N = 27;

int lnk[N];

void dsu_init(int n) {
    for(int i = 0; i < n; ++i) {
        lnk[i] = i;
    }
}

int find(int x) {
    while(x != lnk[x]) x = lnk[x];
    return x;
}

void unite(int a, int b) {
    a = find(a);
    b = find(b);
    if (b<a){
        swap(a,b);
    }
    lnk[b] = a;
}


int main(){

    ios::sync_with_stdio(false);
    cin.tie(0);
    
    string a,b,c;
    cin>>a>>b>>c;
    dsu_init(27);
    for (int i=0; i<(min(a.size(),b.size())); i++){
        unite(a[i]-'a',b[i]-'a');
    }
    string ans="";
    for (int i=0; i<c.size(); i++){
        ans+=(find(c[i]-'a')+'a');
    }
    cout<<ans<<endl;

    return 0;
}
