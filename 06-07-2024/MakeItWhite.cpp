/**
 *    author:  MK_20
 *    created: 2024-07-06 11:58:00
 **/
//Question URL: https://codeforces.com/contest/1927/problem/A
//Rating: 800

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    string s;
    cin>>s;
    int w1=0,w2=0;
    for (int i=0; i<s.size(); i++){
        if (s[i]=='B'){
            break;
        }
        w1++;
    }
    for (int i=n-1; i>=w1; i--){
        if (s[i]=='B'){
            break;
        }
        w2++;
    }
    cout<<min(n, n-w1-w2)<<endl;
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
