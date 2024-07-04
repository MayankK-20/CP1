/**
 *    author:  MK_20
 *    created: 2024-07-04 17:29:04
 **/
//Question URL: https://codeforces.com/problemset/problem/1869/B
//Rating: 1100

#include<bits/stdc++.h>
using namespace std;

void solve(){
    long long n,k,a,b;
    cin>>n>>k>>a>>b;
    a--;b--;
    vector<pair<long long,long long>> v(n);
    for (long long i=0; i<n; i++){
        cin>>v[i].first>>v[i].second;
    }
    long long ad=INT64_MAX;
    long long bd=INT64_MAX;
    for (long long i=0; i<k; i++){
        ad=min(ad, (abs(v[i].first-v[a].first)+abs(v[i].second-v[a].second)));
        bd=min(bd, (abs(v[i].first-v[b].first)+abs(v[i].second-v[b].second)));
    }
    if (ad==INT64_MAX){
        cout<<abs(v[a].first-v[b].first)+abs(v[a].second-v[b].second)<<endl;
        return;
    }
    cout<<min(ad+bd, (abs(v[a].first-v[b].first)+abs(v[a].second-v[b].second)))<<endl;
}

int main(){

    long long t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
