#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <utility>
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

    auto check = [&]()
    {
        Disjoint_set djs(n);
        int vis[3] = {};
        for (int i = 0; i < n; i++)
            vis[clr[i]] = 1;
        for (int i = 0; i < 3; i++)
            if (!vis[i])
                return false;
        for (auto [u, v] : edges)
            if (clr[u] != clr[v] && !djs.Union(u, v))
                return false;
        return true;
    };

    vector<int> idx(n);
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&](int a, int b)
    {
        return G[a].size() > G[b].size();
    });

    if (n > 10) idx.resize(10);

    do {
        int u = 0, v = 0;
        do 
        {
            u = idx[rng() % int(idx.size())];
            v = rng() % n;
        } while (u == v);
        fill_n(clr.begin(), n, 0);
        clr[u] = 1;
        clr[v] = 2;
    } while (!check());

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
