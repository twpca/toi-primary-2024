#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

int main()
{
    ios::sync_with_stdio(0), cin.tie(0);
    int n, k, e;
    cin >> n >> k >> e;
    vector<int> hole(e + 1, 0), steps(k), frm(e + 1, -1);
    while (n--)
    {
        int x;
        cin >> x;
        hole[x] = 1;
    }
    for (int &i : steps)
        cin >> i;
    vector<vector<pair<int, int>>> dp(e + 1);
    for (int i = 1; i <= e; i++)
    {
        if (hole[i]) continue;
        for (int j : steps)
        {
            if (i < j || (i > j && dp[i - j].empty())) continue;
            auto copy = dp[i - j];
            auto it = lower_bound(copy.begin(), copy.end(), make_pair(j, e + 1), greater<>());
            if (it == copy.end() || it->first != j)
                copy.insert(it, make_pair(j, 1));
            else
                it->second += 1;
            if (copy > dp[i])
            {
                dp[i] = copy;
                frm[i] = i - j;
            }
        }
    }
    if (dp[e].empty())
        cout << "-1\n";
    else
    {
        vector<int> ans;
        for (; e; e = frm[e])
            ans.push_back(e);
        reverse(ans.begin(), ans.end());
        cout << ans.size() << "\n";
        for (int i = 0; i < int(ans.size()); i++)
            cout << ans[i] << " \n"[i + 1 == int(ans.size())];
    }
}
