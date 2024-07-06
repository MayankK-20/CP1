/**
 *    author:  MK_20
 *    created: 2024-07-06 11:35:33
 **/
//Question URL: https://codeforces.com/problemset/problem/1969/A
//Rating: 800

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<int> v(n+1);
    for (int i=1; i<n+1; i++){
        cin>>v[i];
    }
    for (int i=1; i<n+1; i++){
        if (v[v[i]]==i){
            cout<<2<<endl;
            return;
        }
    }
    cout<<3<<endl;
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
