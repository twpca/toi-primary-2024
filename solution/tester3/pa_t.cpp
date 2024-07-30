
#include <bits/stdc++.h>

using namespace std;
using LL = int64_t;

LL solutionT(int m, int n, int k, int r, const vector<int> &ones) {
    LL ret=0;
    LL k2 = LL(k)*k;
    if(k2&1) {
        ret += max(0, r-k+1) * LL(n-k+1);
        ret += max(0, m-r-1-k+1) * LL(n-k+1);
    }
    deque<int> window, incoming;
    for(int one: ones)
        incoming.push_back(one);
    int st=0;
    while(!incoming.empty() && incoming.front() < st+k) {
        window.push_back(incoming.front());
        incoming.pop_front();
    }
    int range = min(r,m-k) - max(0,r-k+1) + 1;
    while(true) {
        int next_st = n-k+1;
        if(!window.empty()) {
            next_st = min(next_st, window.front() + 1);
        }
        if(!incoming.empty()) {
            next_st = min(next_st, incoming.front() - k + 1);
        }
        int cnt = window.size();
        if((k2 - cnt)&1)
            ret += LL(next_st - st) * range;
        if(next_st == n-k+1) break;
        st = next_st;
        while(!window.empty() && window.front() < st) window.pop_front();
        while(!incoming.empty() && incoming.front() < st+k) {
            window.push_back(incoming.front());
            incoming.pop_front();
        }
    }
    return ret;
}
int main(void) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    int m, n;
    cin>>m>>n;
    int t, k, r;
    cin>>t>>k>>r;
    r--;
    vector<int> ones(t);
    for(int i=0;i<t;i++) {
        cin>>ones[i];
        ones[i]--;
    }
    LL ans = solutionT(m, n, k, r, ones);
    cout << ans << endl;
    return 0;
}
