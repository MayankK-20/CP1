/**
 *    author:  MK_20
 *    created: 2024-07-21 20:13:25
 **/
//Question URL: https://codeforces.com/group/8bhJZQiaub/contest/537565/problem/C
//Rating:

#include<bits/stdc++.h>
using namespace std;

string a,b,c,d;

int rec(int i, int j, int k, int l, vector<vector<vector<vector<int>>>>& dp){
    if ((i==a.size()) || (j==b.size()) || (k==c.size()) || (l==d.size())){
        // cout<<i<<" "<<j<<" "<<k<<" "<<l<<"\n";
        return 0;
    }
    if (dp[i][j][k][l]!=-1){
        return dp[i][j][k][l];
    }
    if (a[i]==b[j] && b[j]==c[k] && c[k]==d[l]){
        // cout<<"here\n";
        return dp[i][j][k][l]=1+rec(i+1, j+1, k+1, l+1, dp);
    }
    dp[i][j][k][l] = max(rec(i+1,j,k,l,dp),rec(i,j+1,k,l,dp));
    dp[i][j][k][l] = max(dp[i][j][k][l],rec(i,j,k+1,l,dp));
    return dp[i][j][k][l] = max(dp[i][j][k][l], rec(i,j,k,l+1,dp));
}

int main(){

    ios::sync_with_stdio(false);
    cin.tie(0);

    cin>>a>>b>>c>>d;
    vector<vector<vector<vector<int>>>> dp(a.size(), vector<vector<vector<int>>>(b.size(), vector<vector<int>>(c.size(), vector<int>(d.size(),-1))));
    // for (int i=0; i<a.size(); i++){
    //     for (int j=0; j<b.size(); j++){
    //         for (int k=0; k<c.size(); k++){
    //             for (int l=0; l<d.size(); l++){
    //                 cout<<dp[i][j][k][l]<<" ";
    //             }cout<<endl;
    //         }
    //     }
    // }
    cout<<rec(0,0,0,0,dp)<<endl;

    return 0;
}
