#include <iostream>
#include <vector>
#include <numeric>
#include <utility>
#include <random>
#include <queue>
#include <algorithm>
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

mt19937 rng(12345678);

void solve()
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> G(n);
    vector<pair<int, int>> edges(m);
    vector<int> clr(n, 2);
    for (auto &[u, v] : edges)
    {
        cin >> u >> v;
        u--, v--;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    int ansu = -1, ansv = -1;

    // cut vertex
    auto run_cut_vertex = [&]()
    {
        vector<int> dfn(n), low(n);
        int dft = 0;
        auto dfs = [&](auto _dfs, int u, int f) -> void
        {
            dfn[u] = low[u] = ++dft;
            int child = -1;
            for (int i : G[u])
                if (i != f && !dfn[i])
                {
                    _dfs(_dfs, i, u);
                    low[u] = min(low[u], low[i]);
                    if (low[i] >= dfn[u] && u != f)
                    {
                        ansu = f, ansv = i;
                    }
                    else if (child != -1 && u == f)
                    {
                        ansu = child, ansv = i;
                    }
                    child = i;
                }
                else if (i != f && dfn[i] < dfn[u])
                    low[u] = min(low[u], dfn[i]);
        };
        for (int i = 0; i < n; i++)
            if (!dfn[i])
            {
                dfs(dfs, i, i);
                if (ansu != -1) return true;
            }
        return false;
    };

    // bfs
    auto run_bfs = [&]()
    {
        vector<int> idx(n);
        iota(idx.begin(), idx.end(), 0);
        shuffle(idx.begin(), idx.end(), rng);
        if (n > 100) idx.resize(100);
        for (int v : idx)
        {
            queue<int> q;
            vector<int> dis(n, n);
            auto relax = [&](int u, int d)
            {
                if (dis[u] == n)
                {
                    dis[u] = d;
                    q.push(u);
                }
            };

            relax(v, 0);
            while (!q.empty())
            {
                int u = q.front();
                q.pop();
                for (int i : G[u])
                    relax(i, dis[u] + 1);
            }
            int u = max_element(dis.begin(), dis.end()) - dis.begin();
            if (dis[u] >= 3)
            {
                ansu = u, ansv = v; 
                return true;
            }
        }
        return false;
    };

    if (!run_cut_vertex() && !run_bfs())
    {
        cout << "-1\n";
        return;
    }
    clr[ansu] = 0, clr[ansv] = 1;

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
