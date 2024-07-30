#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Binary_Index_Tree 
{
    int n;
    vector<long long> bit;
public:
    Binary_Index_Tree(int _n): n(_n), bit(n + 1) {}
    void modify(int x, long long v) 
    {
        for (; x <= n; x += x & -x)
            bit[x] += v;
    }
    long long query(int x) 
    {
        long long res = 0;
        for (; x; x -= x & -x)
            res += bit[x];
        return res;
    }
};

long long solve(vector<vector<int>> &G1, vector<vector<int>> &G2, int k) 
{
    int n = G1.size() - 1, dft = 0;
    vector<int> dfs_in(n + 1), dfs_out(n + 1), dep(n + 1), father(n + 1), kf(n + 1, -1), stk;
     
    auto dfs = [&](auto _dfs, int u, int f) -> void 
    {
        father[u] = f, dep[u] = dep[f] + 1;
        stk.push_back(u);
        if (int(stk.size()) > k) kf[u] = stk[stk.size() - 1 - k];
        dfs_in[u] = ++dft;
        for (int v : G2[u])
            _dfs(_dfs, v, u);
        dfs_out[u] = dft;
        stk.pop_back();
    };
    dfs(dfs, G2[0][0], G2[0][0]);

    Binary_Index_Tree lft(n), rgt(n);
    long long ans = 0;
    auto cal = [&](auto _cal, int u) -> void 
    {
        int cur = 0;
        if (kf[u] != -1)
        {
            cur += lft.query(dfs_in[kf[u]] - 1);
            cur += rgt.query(n) - rgt.query(dfs_out[kf[u]]);
        }
        ans += cur;

        auto modify = [&](int v) 
        {
            if (kf[u] != -1)
            {
                lft.modify(dfs_out[kf[u]], v);
                rgt.modify(dfs_in[kf[u]], v);
            }
        };

        modify(1);
        for (int i : G1[u])
            _cal(_cal, i);
        modify(-1);
    };

    cal(cal, G1[0][0]);

    return ans;
}

int main() 
{
    ios::sync_with_stdio(0), cin.tie(0);
    int n, k;
    cin >> n >> k;
    vector<vector<int>> G1(n + 1), G2(n + 1);
    auto read = [&](auto &G) 
    {
        for (int i = 1; i <= n; i++) 
        {
            int p;
            cin >> p;
            G[p].push_back(i);
        }
    };
    read(G1), read(G2);
    long long ans = 0;
    ans += solve(G1, G2, k);
    ans += solve(G2, G1, k);
    cout << ans << "\n";
}
