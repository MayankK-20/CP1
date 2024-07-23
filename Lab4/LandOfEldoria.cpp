/**
 *    author:  MK_20
 *    created: 2024-07-21 21:01:01
 **/
//Question URL: https://codeforces.com/group/8bhJZQiaub/contest/537565/problem/B
//Rating:

#include<bits/stdc++.h>
using namespace std;

const int N = (500001);

vector<pair<int,int>> v;

int lnk[N];
int sz[N];

void dsu_init(int n) {
    for(int i = 1; i <= n; ++i) {
        lnk[i] = i;
        sz[i] = 1;
    }
}

int find(int x) {
    if(x != lnk[x]){
        lnk[x]=find(lnk[x]);
    }
    return lnk[x];
}

void unite(int a, int b) {
    int tmp=a,tmp2=b;
    a = find(a);
    b = find(b);
    if (a==b){
        v.push_back({tmp,tmp2});
    }
    if(sz[a] < sz[b]) swap(a, b);
    sz[a] += sz[b];
    lnk[b] = a;
}



int main(){

    ios::sync_with_stdio(false);
    cin.tie(0);
    
    int n;
    cin>>n;
    dsu_init(n);
    for (int i=1; i<n; i++){
        int u,v1;
        cin>>u>>v1;
        unite(u-1,v1-1);
    }
    vector<int> p;
    for (int i=0; i<n; i++){
        if (i==find(i)){
            p.push_back(i);
        }
    }
    cout<<v.size()<<endl;
    for (int i=0; i<v.size(); i++){
        cout<<v[i].first+1<<" "<<v[i].second+1<<" "<<p[i]+1<<" "<<p[i+1]+1<<endl;
    }

    return 0;
}
