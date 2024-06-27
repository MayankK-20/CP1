/**
 *    author:  MK_20
 *    created: 2024-06-25 20:13:42
 **/
//Question URL: https://codeforces.com/contest/1982/problem/B
//Rating: 1200

#include<iostream>
#include<numeric>
#include<vector>
#include<algorithm>
using namespace std;

void solve(){
    int x,y,k;
    cin>>x>>y>>k;
    while (true){
        int tmp=x;
        int operations=(y-(tmp%y));
        // cout<<operations<<" ";
        if (k<operations){
            cout<<x+k<<endl;
            return;
        }
        k-=operations;
        tmp+=operations;
        while(!(tmp%y)){
            tmp/=y;
        }
        if (tmp==x){
            k=(k%operations);
        }
        x=tmp;
    }
}

int main(){

    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
