/**
 *    author:  MK_20
 *    created: 2024-07-02 16:44:09
 **/
//Question URL: https://codeforces.com/problemset/problem/1948/C
//Rating: 1300

#include<bits/stdc++.h>
using namespace std;

bool dfst(int i, int j, vector<vector<char>>& v, vector<vector<bool>>& visited, int n){
    if (visited[i][j]){
        return false;
    }
    visited[i][j]=true;
    if (i==1 && j==n-1){
        return true;
    }
    if (i==0 && j==n-1){
        return false;
    }
    visited[i][j+1]=true;
    if (dfst(i, j+(v[i][j+1]=='<'? 0:2), v, visited, n)){return true;}
    if (i==0){
        visited[i+1][j]=true;
        if (dfst(i+1, j+(v[i+1][j]=='<'?-1:1), v, visited, n)){return true;}
    }
    else{
        visited[i-1][j]=true;
        if (dfst(i-1, j+(v[i-1][j]=='<'?-1:1),v,visited,n)){return true;};
    }
    return false;
}

void solve(){
    int n;
    cin>>n;
    vector<vector<char>> v(2, vector<char>(n));
    for (int i=0; i<2; i++){
        for (int j=0; j<n; j++){
            cin>>v[i][j];
        }
    }
    vector<vector<bool>> visited(2, vector<bool>(n,false));
    if (dfst(0,0,v,visited,n)){
        cout<<"YES\n";
        return;
    }
    cout<<"NO\n";
}

int main(){

    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
