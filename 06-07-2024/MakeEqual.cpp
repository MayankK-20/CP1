/**
 *    author:  MK_20
 *    created: 2024-07-06 21:08:47
 **/
//Question URL: https://codeforces.com/contest/1931/problem/B
//Rating: 800

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<int> v(n);
    long long sum=0;
    for (int i=0; i<n; i++){
        cin>>v[i];
        sum+=v[i];
    }
    sum/=n;
    for (int i=n-1; i>0; i--){
        if (v[i]>sum){
            cout<<"NO\n";
            return;
        }
        v[i-1]=v[i-1]-(sum-v[i]);
    }
    cout<<"YES\n";
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
