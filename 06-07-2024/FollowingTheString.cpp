/**
 *    author:  MK_20
 *    created: 2024-07-06 12:06:07
 **/
//Question URL: https://codeforces.com/contest/1927/problem/B
//Rating: 900

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<int> v(26, 0);
    vector<char> ans(n);
    for (int j=0; j<n; j++){
        int u;
        cin>>u;
        for (int i=0; i<26; i++){
            if (v[i]==u){
                v[i]++;
                ans[j]=('a'+i);
                break;
            }
        }
    }
    for (int i=0; i<n; i++){
        cout<<ans[i];
    }cout<<endl;
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
