/**
 *    author:  MK_20
 *    created: 2024-07-06 08:36:28
 **/
//Question URL: https://codeforces.com/problemset/problem/1557/A
//Rating: 800

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<int> v(n);
    int maxim=INT_MIN;
    long long sum=0;
    for (int i=0; i<n; i++){
        cin>>v[i];
        maxim=max(maxim, v[i]);
        sum+=v[i];
    }
    sum-=maxim;
    cout.precision(6);
    cout<<fixed<<maxim+((double)sum/(n-1))<<endl;
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
