/**
 *    author:  MK_20
 *    created: 2024-07-04 20:16:07
 **/
//Question URL: https://codeforces.com/group/8bhJZQiaub/contest/533994/problem/A
//Rating:
 
#include<bits/stdc++.h>
using namespace std;
 
void solve(){
    int n,m;
    cin>>n>>m;
    vector<vector<int>> v(n);
    for (int i=0; i<n; i++){
        int u,v1;
        cin>>u>>v1;
        v[u-1].push_back(v1-1);
        v[v1-1].push_back(u-1);
    }
    long long ans=0;
    for (int i=0; i<n; i++){
        ans+=v[i].size();
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
