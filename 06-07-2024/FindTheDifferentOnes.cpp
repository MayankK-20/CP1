/**
 *    author:  MK_20
 *    created: 2024-07-06 12:42:41
 **/
//Question URL: https://codeforces.com/contest/1927/problem/D
//Rating: 1300

#include<bits/stdc++.h>
using namespace std;

void solve(){
    int n;
    cin>>n;
    vector<int> v(n);
    for (int i=0; i<n; i++){
        cin>>v[i];
    }
    // vector<int> s(n, INT_MAX);
    vector<int> f(n, INT_MAX);
    {
        int i=0;
        while(i<n-1){
            if (v[i+1]!=v[i]){
                f[i]=i+1;
                i++;
            }
            else{
                if (i+1==n-1){
                    break;
                }
                for (int j=i+1; j<n-1; j++){
                    if (v[j+1]!=v[j]){
                        for (int k=i; k<=j; k++){
                            f[k]=j+1;  
                        }
                        i=j+1;
                        break;
                    }
                    if (j==n-2){
                        i=j+1;
                    }
                }
            }
            
        }
        // i=n-1;
        // while (i>0){
        //     if (v[i]!=v[i-1]){
        //         s[i]=i-1;
        //         i--;
        //     }
        //     else{
        //         for (int j=i-1; j>0; j--){
        //             if (v[j-1]!=v[j]){
        //                 for (int k=i; k>=j; k--){
        //                     s[k]=j-1;
        //                 }
        //                 i=j-1;
        //                 break;
        //             }
        //             if (j==1){
        //                 i=0;
        //             }
        //         }
        //     }
        // }
    }
    int q;
    cin>>q;
    for (int i=0; i<q; i++){
        int l,r;
        cin>>l>>r;
        if (f[l-1]<r){
            cout<<l<<" "<<(f[l-1]+1)<<endl;
        }
        else{
            cout<<-1<<" "<<-1<<endl;
        }
    }
    cout<<endl;
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
