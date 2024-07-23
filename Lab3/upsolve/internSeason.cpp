/**
 *    author:  MK_20
 *    created: 2024-07-04 22:48:34
 **/
//Question URL: https://codeforces.com/group/8bhJZQiaub/contest/533994/problem/C
//Rating:

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    if (n<3){
        cout<<0<<endl;
        return;
    }
    if (n==3){
        cout<<1<<endl;
        return;
    }
    cout<<n-2<<endl;
    
    // int st=n/2, end=n;
    // int ans=st;
    // while (st<=end){
    //     int mid=(end-st)/2+st;
    //     int k=mid/(n-mid);
    //     if (mid-1>k){
    //         if (ans<mid){
    //             ans=mid;
    //             st=mid+1;
    //         }
    //         else{
    //             end=mid-1;
    //         }
    //     }
    //     else{
    //         end=mid-1;
    //     }
    // }
    // cout<<ans<<endl;
}

int main(){

    int t;
    cin>>t;
    while(t--){
        solve();
    }

    return 0;
}
