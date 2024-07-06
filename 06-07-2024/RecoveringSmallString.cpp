/**
 *    author:  MK_20
 *    created: 2024-07-06 20:22:57
 **/
//Question URL: https://codeforces.com/contest/1931/problem/A
//Rating: 800

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<char> ans(3);
    ans[2]=min(26,n-2)-1+'a';
    n-=(min(26, n-2));
    ans[1]=min(26, n-1)-1+'a';
    n-=min(26, n-1);
    ans[0]='a'+n-1;
    cout<<ans[0]<<ans[1]<<ans[2]<<endl;
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
