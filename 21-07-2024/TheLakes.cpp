/**
 *    author:  MK_20
 *    created: 2024-07-21 18:05:09
 **/
//Question URL: https://codeforces.com/problemset/problem/1829/E
//Rating: 1100

#include<bits/stdc++.h>
using namespace std;

int n,m;

long long dfst(int i, int j, vector<vector<int>>& v){
    if (i<0 || i==n || j<0 || j==m || !v[i][j]){
        return 0ll;
    }
    long long cur=v[i][j];
    v[i][j]=0;
    cur+=dfst(i+1,j,v);
    cur+=dfst(i,j+1,v);
    cur+=dfst(i,j-1,v);
    cur+=dfst(i-1,j,v);
    return cur;
}

void solve(){

    cin>>n>>m;
    vector<vector<int>> v(n, vector<int>(m));
    for (int i=0; i<n; i++){
        for (int j=0; j<m; j++){
            cin>>v[i][j];
        }
    }
    long long ans=0;
    for (int i=0; i<n; i++){
        for (int j=0; j<m; j++){
            ans=max(ans,dfst(i,j,v));
        }
    }
    cout<<ans<<endl;
}

int main(){

    ios::sync_with_stdio(false);
    cin.tie(0);
    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
