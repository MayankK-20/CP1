/**
 *    author:  MK_20
 *    created: 2024-06-25 21:02:31
 **/
//Question URL: https://codeforces.com/contest/1982/problem/C
//Rating: 1200

#include<iostream>
#include<numeric>
#include<vector>
#include<algorithm>
using namespace std;

void solve(){
    int n,l,r;
    cin>>n>>l>>r;
    vector<int> v(n);
    for (int i=0; i<n; i++){
        cin>>v[i];
    }
    long long sum=0;
    int ans=0;
    int st=0;
    for (int i=0; i<n; i++){
        if (v[i]<=r && v[i]>=l){
            ans++;
            sum=0;
            st=i+1;
            continue;
        }
        if (v[i]>r){
            sum=0;
            st=i+1;
            continue;
        }
        sum+=v[i];
        if (sum<=r && sum>=l){
            ans++;
            sum=0;
            st=i+1;
        }
        else if (sum>r){
            for (int j=st; j<i; j++){
                sum-=v[j];
                if (sum<=r && sum>=l){
                    ans++;
                    sum=0;
                    st=i+1;
                    break;
                }
                if (sum<l){
                    st=j+1;
                    break;
                }
            }
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
