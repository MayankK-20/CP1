/**
 *    author:  MK_20
 *    created: 2024-06-28 15:31:11
 **/
//Question URL: https://codeforces.com/problemset/problem/1976/A
//Rating: 800

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<char> a(n);
    vector<char> b(n);
    for (int i=0; i<n; i++){
        cin>>a[i];
        b[i]=a[i];
    }
    sort(b.begin(), b.end());
    cout<<(a==b? "YES\n":"NO\n");
}

int main(){

    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
