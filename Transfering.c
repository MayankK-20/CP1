/**
 *    author:  MK_20
 *    created: 2024-11-09 20:09:24
 **/
//Question URL:
//Rating:

#include<bits/stdc++.h>
#define ll long long
#define pii pair<int,int>
using namespace std;

bool better(int arr[],int brr[]){
    bool more=false;
    for (int i=0; i<3; i++){
        if (arr[i]>brr[i]){
            more=true;
        }
        else if (arr[i]==brr[i]){
            continue;
        }
        else{
            return false;
        }
    }
    return more;
}

void solve(){
    int arr[3];
    int brr[3];
    int crr[3];
    cin>>arr[0]>>arr[1]>>arr[2];
    cin>>brr[0]>>brr[1]>>brr[2];
    cin>>crr[0]>>crr[1]>>crr[2];
    if ((!better(arr,brr) && !better(brr,arr)) || (!better(brr,crr) && !better(crr,brr)) || (!better(arr,crr) && !better(crr,arr))){
        cout<<"no\n";
        return;
    }
    cout<<"yes\n";
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
