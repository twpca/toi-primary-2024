#include <iostream>
#include <vector>
using namespace std;

long long solve(vector<vector<int>> &G1, vector<vector<int>> &G2, int k) 
{
    int n = G1.size() - 1;
    vector<int> father1(n + 1), father2(n + 1), dep1(n + 1), dep2(n + 1);
    auto dfs = [&](auto _dfs, auto &G, auto &father, auto &dep, int u, int f) -> void 
    {
        father[u] = f, dep[u] = dep[f] + 1;
        for (int v : G[u])
            _dfs(_dfs, G, father, dep, v, u);
    };
    dfs(dfs, G1, father1, dep1, G1[0][0], 0);
    dfs(dfs, G2, father2, dep2, G2[0][0], 0);

    auto get_dis2 = [&](int u, int v) 
    {
        int ans = 0;
        while (u != v) 
        {
            if (dep2[u] < dep2[v]) swap(u, v);
            if (dep2[u] == dep2[v]) 
            { 
                u = father2[u];
                v = father2[v];
                ans++;
            }
            else 
            {
                u = father2[u];
            }
        }
        return ans;
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
