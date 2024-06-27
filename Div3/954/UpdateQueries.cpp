/**
 *    author:  MK_20
 *    created: 2024-06-23 21:13:14
 **/
//Question URL: https://codeforces.com/problemset/problem/1986/C
//Rating: 1100

#include<iostream>
#include<numeric>
#include<vector>
#include<algorithm>
using namespace std;

void solve(){
    int n,m;
    cin>>n>>m;
    vector<int> cnt(n+1, 0);
    string s;
    cin>>s;
    vector<char> str(m);
    for (int i=0; i<m; i++){
        int inp;
        cin>>inp;
        cnt[inp]++;
    }
    for (int i=0; i<m; i++){
        cin>>str[i];
    }
    int j=0;
    sort(str.begin(), str.end());
    string ans="";
    for (int i=0; i<n; i++){
        if (cnt[i+1]){
            ans+=str[j];
            j++;
        }
        else{
            ans+=s[i];
        }
    }
    cout<<ans<<endl;
}

int main(){

    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
