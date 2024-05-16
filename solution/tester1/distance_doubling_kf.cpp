#include <iostream>
#include <vector>
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
    int n = G1.size() - 1, dft = 0, lgn = __lg(n);
    vector<int> dfs_in(n + 1), dfs_out(n + 1), dep(n + 1);
    vector<vector<int>> father(lgn + 1, vector<int>(n + 1));
     
    auto dfs = [&](auto _dfs, int u, int f) -> void 
    {
        father[0][u] = f, dep[u] = dep[f] + 1;
        for (int i = 1; i <= lgn; i++)
            father[i][u] = father[i - 1][father[i - 1][u]];
        dfs_in[u] = ++dft;
        for (int v : G2[u])
            _dfs(_dfs, v, u);
        dfs_out[u] = dft;
    };
    dfs(dfs, G2[0][0], G2[0][0]);
    auto get_kth_father = [&](int u, int kth) 
    {
        for (int i = lgn; i >= 0; i--)
            if (kth >> i & 1)
                u = father[i][u];
        return u;
    };

    Binary_Index_Tree case1(n), case2(n), case3_1(n), case3_2(n);
    long long ans = 0;
    int counter = 0;
    auto cal = [&](auto _cal, int u) -> void 
    {
        int cur = counter++;

        int kf = get_kth_father(u, k);
        cur -= case1.query(dfs_out[kf]) - case1.query(dfs_in[kf] - 1);
        cur -= case2.query(dfs_in[u]);
        cur += case3_1.query(dfs_out[kf]) - case3_1.query(dfs_in[kf] - 1);
        cur += case3_2.query(dfs_in[u]);
        cur -= case3_2.query(dfs_in[kf]);
        ans += cur;

        auto modify = [&](int v) 
        {
            case1.modify(dfs_in[u], v);
            case2.modify(dfs_in[kf], v), case2.modify(dfs_out[kf] + 1, -v);
            case3_1.modify(dfs_in[u], v);
            if (father[0][kf] != kf)
                case3_1.modify(dfs_in[father[0][kf]], -v);
            case3_2.modify(dfs_in[kf], v), case3_2.modify(dfs_out[kf] + 1, -v);
        };

        modify(1);
        for (int i : G1[u])
            _cal(_cal, i);
        modify(-1), --counter;
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
