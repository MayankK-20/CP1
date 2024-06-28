/**
 *    author:  MK_20
 *    created: 2024-06-28 15:37:27
 **/
//Question URL: https://codeforces.com/problemset/problem/1975/B
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
    sort(v.begin(), v.end());
    int first=v[0];
    int second;
    int idx=-1;
    for (int i=1; i<n; i++){
        if (v[i]%first){
            second=v[i];
            idx=i+1;
            break;
        }
    }
    if (idx==-1){
        cout<<"YES\n";
        return;
    }
    for (int i=idx; i<n; i++){
        if (v[i]%first && v[i]%second){
            cout<<"NO\n";
            return;
        }
    }
    cout<<"YES\n";
}

int main(){

    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
