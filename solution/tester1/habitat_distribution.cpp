#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

struct Disjoint_set
{
    int n;
    vector<int> boss;
    Disjoint_set(int _n): n(_n), boss(n) 
    {
        iota(boss.begin(), boss.end(), 0);
    }
    int get_boss(int x)
    {
        if (x == boss[x]) return x;
        return boss[x] = get_boss(boss[x]);
    }
    bool Union(int x, int y)
    {
        x = get_boss(x), y = get_boss(y);
        if (x == y) return false;
        boss[x] = y;
        return true;
    }
};

void solve()
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> G(n);
    vector<int> clr(n, 2), vis(n);
    while (m--)
    {
        int u, v;
        cin >> u >> v;
        u--, v--;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    int current_color = 0, dft = 0;
    Disjoint_set djs(n);
    auto dfs = [&](auto _dfs, int u, int f) -> void
    {
        vis[u] = ++dft;
        clr[u] = current_color;
        for (int i : G[u])
            if (i != f && !vis[i])
                _dfs(_dfs, i, u);
            else if (i != f && vis[i] < vis[u])
                djs.Union(i, u); 
    };
    for (int i = 0; i < n; i++)
        if (!vis[i])
        {
            int tmp_dft = dft;
            dfs(dfs, i, i);
            if (dft - tmp_dft == 1)
            {
                current_color += 1;
                if (current_color == 2) break;
            }
            else 
            {
                if (dft - tmp_dft == n)
                {
                    for (int j = 0; j < n; j++)
                        if (djs.get_boss(j) == j)
                        {
                            clr[j] = current_color++;
                            if (current_color == 3) break;
                        }
                    for (int j = 0; j < n; j++)
                        clr[j] = clr[djs.get_boss(j)];
                }
                else
                {
                    clr[i] = current_color + 1;
                }
                break; 
            }
        }

    vector<int> ans[3];
    for (int i = 0; i < n; i++)
        ans[clr[i]].push_back(i);
    for (int i = 0; i < 3; i++)
    {
        cout << int(ans[i].size());
        for (int j : ans[i])
            cout << " " << j + 1;
        cout << "\n";
    }
}

int main() 
{
    ios::sync_with_stdio(0), cin.tie(0);
    int t;
    cin >> t;
    while (t--)
    {
        solve();
    }
}
