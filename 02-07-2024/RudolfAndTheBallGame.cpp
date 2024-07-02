/**
 *    author:  MK_20
 *    created: 2024-07-02 15:30:35
 **/
//Question URL: https://codeforces.com/problemset/problem/1941/D
//Rating: 1200

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n,m,x;
    cin>>n>>m>>x;
    set<int> q;
    set<int> nxt;
    q.insert(x);
    for (int i=0; i<m; i++){
        int r;
        char d;
        cin>>r>>d;
        if (d=='0'){
            for (auto el: q){
                nxt.insert((el+r>n? el+r-n: el+r));
            }
            q=nxt;
            nxt.clear();
        }
        else if (d=='1'){
            for (auto el: q){
                nxt.insert((el-r<1? el-r+n: el-r));
            }
            q=nxt;
            nxt.clear();
        }
        else{
            for (auto el: q){
                nxt.insert((el+r>n? el+r-n: el+r));
                nxt.insert((el-r<1? el-r+n: el-r));
            }
            q=nxt;
            nxt.clear();
        }
    }
    cout<<q.size()<<endl;
    for (auto el: q){
        cout<<el<<" ";
    }cout<<endl;
}

int main(){

    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
