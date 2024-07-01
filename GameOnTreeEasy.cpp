/**
 *    author:  MK_20
 *    created: 2024-07-01 22:45:31
 **/
//Question URL: https://codeforces.com/problemset/problem/1970/C1
//Rating: 1400

#include<bits/stdc++.h>
using namespace std;

void dfs(int st, vector<vector<int>>& v, vector<int>& d, vector<bool>& visited, int depth=0){
    visited[st]=true;
    if (v[st].size()==1){
        d.push_back(depth);
    }
    for (int i=0; i<v[st].size(); i++){
        if (!visited[v[st][i]]){
            dfs(v[st][i], v, d, visited, depth+1);
        }
    }
}

int main(){

    int n, t;
    cin>>n>>t;
    vector<vector<int>> v(n, vector<int>(0));
    for (int i=1; i<n; i++){
        int u,j;
        cin>>u>>j;
        v[u-1].push_back(j-1);
        v[j-1].push_back(u-1);
    }
    int st;
    cin>>st;
    st--;
    vector<int> d;
    vector<bool> visited(n, false);
    dfs(st, v, d, visited);
    if (d[0]%2 || d[1]%2){
        cout<<"Ron\n";
    }
    else{
        cout<<"Hermione\n";
    }

    return 0;
}
