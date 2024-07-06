/**
 *    author:  MK_20
 *    created: 2024-07-06 12:19:20
 **/
//Question URL: https://codeforces.com/contest/1927/problem/C
//Rating: 1000

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n,m,k;
    cin>>n>>m>>k;
    vector<int> a(n);
    vector<int> b(m);
    set<int> f;
    set<int> s;
    vector<int> have(k+1, false);
    for (int i=0; i<n; i++){
        cin>>a[i];
        if (a[i]<=k){
            f.insert(a[i]);
            have[a[i]]=true;
        }
    }
    for (int i=0; i<m; i++){
        cin>>b[i];
        if (b[i]<=k){
            s.insert(b[i]);
            have[b[i]]=true;
        }
    }
    for (int i=1; i<=k; i++){
        if (!have[i]){
            cout<<"NO\n";
            return;
        }
    }
    if (f.size()>=k/2 && s.size()>=k/2){
        cout<<"YES\n";
        return;
    }
    cout<<"NO\n";
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
