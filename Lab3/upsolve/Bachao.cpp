/**
 *    author:  MK_20
 *    created: 2024-07-04 20:16:07
 **/
//Question URL: https://codeforces.com/group/8bhJZQiaub/contest/533994/problem/B
//Rating:

#include<bits/stdc++.h>
using namespace std;

int mod=(1e9+7);

int dfs(int source, vector<vector<int>>& v, vector<bool>& visited){
    if (visited[source]){
        return 0;
    }
    visited[source]=true;
    int ans=0;
    int size=v[source].size();
    for (int i=0; i<size; i++){
        if (!visited[v[source][i]]){
            ans+=dfs(v[source][i], v, visited);
        }
    }
    return 1+ans;
}

void solve(){
    int n;
    long long m;
    cin>>n>>m;
    vector<vector<int>> v(n);
    for (long long i=0; i<m; i++){
        int u,v1;
        cin>>u>>v1;
        v[u-1].push_back(v1-1);
        v[v1-1].push_back(u-1);
    }
    int paths=0;
    long long ans=1;
    vector<bool> visited(n, false);
    for (int i=0; i<n; i++){
        int tmp=dfs(i, v, visited);
        if (tmp){
            paths=(paths+1)%mod;
            ans=(ans*tmp)%mod;
        }
    }
    cout<<paths%mod<<" "<<ans<<endl;
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
