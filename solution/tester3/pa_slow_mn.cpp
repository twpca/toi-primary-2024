#include <bits/stdc++.h>

using namespace std;
using LL = int64_t;

int solutionMN(int m, int n, int k, int r, const vector<int> &ones) {
    int ret=0;
    vector<int> prefix_line(n, 0);
    for(int one: ones)
        prefix_line[one] = 1;
    for(int i=1;i<n;i++)
        prefix_line[i] += prefix_line[i-1];
    int k2 = k*k;
    for(int i=0;i+k-1<m;i++)
        for(int j=0;j+k-1<n;j++){
            if(i>r || i+k-1<r){
                ret += k2&1;
            } else {
                int cnt = prefix_line[j+k-1] - (j ? prefix_line[j-1] : 0);
                ret +=  (k2 - cnt)&1;
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
    int ans = solutionMN(m, n, k, r, ones);
    cout << ans << endl;
    return 0;
}
