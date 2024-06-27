/**
 *    author:  MK_20
 *    created: 2024-06-23 20:22:04
 **/
//Question URL: https://codeforces.com/contest/1986/problem/A
//Rating: 800

#include<iostream>
#include<numeric>
#include<vector>
#include<algorithm>
#include<climits>
using namespace std;

void solve(){
    int x1,x2,x3;
    cin>>x1>>x2>>x3;
    int ans=INT_MAX;
    for (int i=1; i<11; i++){
        ans=min(ans, abs(x1-i)+abs(x2-i)+abs(x3-i));
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
