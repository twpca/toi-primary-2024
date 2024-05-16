#include <bits/stdc++.h>

using namespace std;
using LL = int64_t;

LL solutionN(int m, int n, int k, int r, const vector<int> &ones) {
    LL ret=0;
    vector<int> prefix_line(n, 0);
    for(int one: ones)
        prefix_line[one] = 1;
    for(int i=1;i<n;i++)
        prefix_line[i] += prefix_line[i-1];
    LL k2 = LL(k)*k;
    if(k2&1) {
        ret += max(0, r-k+1) * LL(n-k+1);
        ret += max(0, m-r-1-k+1) * LL(n-k+1);
    }
    int range = min(r,m-k) - max(0,r-k+1) + 1;
    for(int j=0;j+k-1<n;j++){
        int cnt = prefix_line[j+k-1] - (j ? prefix_line[j-1] : 0);
        if((k2 - cnt)&1)
            ret += range;
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
    LL ans = solutionN(m, n, k, r, ones);
    cout << ans << endl;
    return 0;
}
