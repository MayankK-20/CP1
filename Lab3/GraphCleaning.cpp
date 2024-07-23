/**
 *    author:  MK_20
 *    created: 2024-07-04 21:10:08
 **/
//Question URL: https://codeforces.com/group/8bhJZQiaub/contest/533994/problem/D
//Rating:

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n,m,l;
    cin>>n>>m>>l;
    vector<vector<int>> v(n);
    for (int i=0; i<m; i++){
        int u,v1;
        cin>>u>>v1;
        v[u-1].push_back(v1-1);
        v[v1-1].push_back(u-1);
    }
    vector<int> cnt(n);
    vector<bool> visited(n, false);
    for (int i=0; i<n; i++){
        visited[i]=true;
        for (int j=0; j<v[i].size(); j++){
            if (visited[v[i][j]]){
                continue;
            }
            for (int k=j+1; k<v[i].size(); k++){
                if (visited[v[i][k]]){
                    continue;
                }
                if (find(v[v[i][j]].begin(), v[v[i][j]].end(), v[i][k])!=v[v[i][j]].end()){
                    cnt[i]++;
                    cnt[v[i][j]]++;
                    cnt[v[i][k]]++;
                }
            }
        }
    }
    int ans=0;
    for (int i=0; i<n; i++){
        if (cnt[i]<l){
            ans++;
        }
    }
    cout<<ans<<endl;
}

int main(){

    int t=1;
    // cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
