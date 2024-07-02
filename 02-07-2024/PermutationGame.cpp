/**
 *    author:  MK_20
 *    created: 2024-07-02 17:24:26
 **/
//Question URL: https://codeforces.com/problemset/problem/1968/D
//Rating: 1300

#include<bits/stdc++.h>
using namespace std;

void dfst(long long i, long long k, vector<long long>& p, vector<long long>& a, long long& answer, vector<bool>& visited, long long score){
    if (visited[i]){
        return;
    }
    visited[i]=true;
    if (k==0){
        return;
    }
    if (answer<(score+(k*a[i]))){
        answer=(score+(k*a[i]));
    }
    dfst(p[i],k-1,p,a,answer,visited,score+a[i]);
}

void solve(){
    long long n,k,b,s;
    cin>>n>>k>>b>>s;
    b--;
    s--;
    vector<long long> p(n);
    vector<long long> a(n);
    long long bodya=0;
    long long sasha=0;
    for (long long i=0; i<n; i++){
        cin>>p[i];
        p[i]--;
    }
    for (long long i=0; i<n; i++){
        cin>>a[i];
    }
    {
        vector<bool> visited(n, false);
        dfst(b, k, p, a, bodya, visited, 0);
    }
    {
        vector<bool> visited(n, false);
        dfst(s, k, p, a, sasha, visited, 0);
    }
    if (bodya>sasha){
        cout<<"Bodya\n";
    }
    else if (sasha>bodya){
        cout<<"Sasha\n";
    }
    else{
        cout<<"Draw\n";
    }
}

int main(){

    long long t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
