/**
 *    author:  MK_20
 *    created: 2024-07-06 21:24:15
 **/
//Question URL: https://codeforces.com/contest/1931/problem/C
//Rating: 1000

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<int> v(n);
    for (int i=0; i<n; i++){
        cin>>v[i];
    }
    int s=-1, e=0;
    for (int i=0; i<n-1; i++){
        if (v[i]!=v[i+1]){
            s=i+1;
            break;
        }
    }
    if (s<0){
        cout<<"0\n";
        return;
    }
    for (int i=n-1; i>s-1; i--){
        if (v[i]!=v[i-1]){
            e=n-i;
            break;
        }
    }
    if (v[0]==v[n-1]){
        s+=e;
    }
    // cout<<"s: "<<s<<" e: "<<e<<"\n";
    cout<<n-max(s, e)<<endl;
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
