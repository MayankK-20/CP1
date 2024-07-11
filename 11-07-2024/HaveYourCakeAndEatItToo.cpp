    /**
     *    author:  MK_20
     *    created: 2024-07-07 21:06:42
     **/
    //Question URL: https://codeforces.com/contest/1983/problem/C#
    //Rating:

    #include<bits/stdc++.h>
    using namespace std;

    void solve(){
        long long n;
        cin>>n;
        map<long long,vector<long long>> m;
        for (long long i=0; i<3; i++){
            long long u;
            cin>>u;
            m[i].push_back(u);
            for (long long j=1; j<n; j++){
                cin>>u;
                m[i].push_back(u+m[i][j-1]);
            }
        }
        long long big=(m[0][n-1]/3)+(m[0][n-1]%3? 1: 0);
        vector<pair<long long,long long>> ans(3);
        for (long long i=0; i<3; i++){
            ans[i].first=0;
            ans[i].second=lower_bound(m[i].begin(), m[i].end(), big)-m[i].begin();
            ans[(i+1)%3].first=ans[i].second+1;
            ans[(i+1)%3].second=lower_bound(m[(i+1)%3].begin(), m[(i+1)%3].end(), big+(m[(i+1)%3][ans[i].second]))-m[(i+1)%3].begin();
            ans[(i+2)%3].first=ans[(i+1)%3].second+1;
            ans[(i+2)%3].second=lower_bound(m[(i+2)%3].begin(), m[(i+2)%3].end(), big+(m[(i+2)%3][ans[(i+1)%3].second]))-m[(i+2)%3].begin();
            if (ans[0].second<n && ans[1].second<n && ans[2].second<n){
                cout<<ans[0].first+1<<" "<<ans[0].second+1<<" "<<ans[1].first+1<<" "<<ans[1].second+1<<" "<<ans[2].first+1<<" "<<ans[2].second+1<<endl;
                return;
            }
            ans[(i+2)%3].first=ans[i].second+1;
            ans[(i+2)%3].second=lower_bound(m[(i+2)%3].begin(), m[(i+2)%3].end(), big+(m[(i+2)%3][ans[i].second]))-m[(i+2)%3].begin();
            ans[(i+1)%3].first=ans[(i+2)%3].second+1;
            ans[(i+1)%3].second=lower_bound(m[(i+1)%3].begin(), m[(i+1)%3].end(), big+(m[(i+1)%3][ans[(i+2)%3].second]))-m[(i+1)%3].begin();
            if (ans[0].second<n && ans[1].second<n && ans[2].second<n){
                cout<<ans[0].first+1<<" "<<ans[0].second+1<<" "<<ans[1].first+1<<" "<<ans[1].second+1<<" "<<ans[2].first+1<<" "<<ans[2].second+1<<endl;
                return;
            }
        }
        cout<<-1<<endl;
    }

    int main(){

        ios::sync_with_stdio(false);
        cin.tie(0);
        long long t;
        cin>>t;
        while(t--){
            solve();
        }

        return 0;
    }
