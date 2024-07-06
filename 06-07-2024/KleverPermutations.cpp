/**
 *    author:  MK_20
 *    created: 2024-07-06 13:36:19
 **/
//Question URL: https://codeforces.com/contest/1927/problem/E
//Rating: 1400

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n,k;
    cin>>n>>k;
    int s=1, e=n;
    int i=0, j=1;
    vector<int> ans(n);
    bool inc=true;
    while (e>=s){
        if (inc){
            for (int tmp=i; tmp<n; tmp+=k){
                ans[tmp]=s;
                s++;
            }
            i+=2;
            inc=false;
        }
        else{
            for (int tmp=j; tmp<n; tmp+=k){
                ans[tmp]=e;
                e--;
            }
            j+=2;
            inc=true;
        }
    }
    for (int i=0; i<n; i++){
        cout<<ans[i]<<" ";
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
