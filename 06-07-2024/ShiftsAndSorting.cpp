/**
 *    author:  MK_20
 *    created: 2024-07-06 11:26:45
 **/
//Question URL: https://codeforces.com/problemset/problem/1969/B
//Rating: 1000

#include<bits/stdc++.h>
using namespace std;

void solve(){
    string s;
    cin>>s;
    int first=s.size();
    for (int i=0; i<s.size(); i++){
        if (s[i]=='1'){
            first=i;
            break;
        }
    }
    long long ans=0;
    for (int i=first+1; i<s.size(); i++){
        if (s[i]=='0'){
            ans+=(i-first+1);
            first++;
        }
    }
    cout<<ans<<endl;
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
