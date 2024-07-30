#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

long long solve(vector<vector<int>> &G1, vector<vector<int>> &G2, int k) 
{
    int n = G1.size() - 1, L = __lg(n);
    vector<int> father1(n + 1), father2(n + 1), dep1(n + 1), dep2(n + 1);
    vector<vector<int>> father2_table(L + 1, vector<int>(n + 1));
    auto dfs = [&](auto _dfs, auto &G, auto &father, auto &dep, int u, int f) -> void 
    {
        father[u] = f, dep[u] = dep[f] + 1;
        for (int v : G[u])
            _dfs(_dfs, G, father, dep, v, u);
    };
    dfs(dfs, G1, father1, dep1, G1[0][0], 0);
    dfs(dfs, G2, father2, dep2, G2[0][0], 0);
    for (int i = 1; i <= n; i++)
        father2_table[0][i] = father2[i] ? father2[i] : i;
    for (int i = 1; i <= L; i++)
        for (int j = 1; j <= n; j++)
            father2_table[i][j] = father2_table[i - 1][father2_table[i - 1][j]];

    auto get_lca = [&](int u, int v)
    {
        if (dep2[u] < dep2[v]) swap(u, v);
        int diff = dep2[u] - dep2[v];
        for (int i = L; i >= 0; --i)
            if (diff >> i & 1)
                u = father2_table[i][u];
        if (u == v) return u;
        for (int i = L; i >= 0; --i)
            if (father2_table[i][u] != father2_table[i][v])
            {
                u = father2_table[i][u];
                v = father2_table[i][v];
            }
        return father2_table[0][u];
    };

    auto get_dis2 = [&](int u, int v) 
    {
        int lca = get_lca(u, v);
        return min(dep2[u] - dep2[lca], dep2[v] - dep2[lca]);
    };

    long long ans = 0;
    for (int i = 1; i <= n; i++) 
    {
        int cur = 0;
        for (int x = father1[i]; x; x = father1[x]) 
        {
            if (get_dis2(i, x) > k)
                cur++;
        }
        ans += cur;
    }
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
