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
    sort(steps.begin(), steps.end(), greater<int>());
    vector<vector<int>> dp(e + 1);
    frm[0] = 0;
    for (int i = 0; i < e; i++)
    {
        if (hole[i] || frm[i] == -1) continue;
        vector<int> tmp(dp[i]);
        for (int j = 0; j < k; j++)
        {
            if (i + steps[j] <= e && !hole[i + steps[j]])
            {
                if (int(tmp.size()) <= j) tmp.resize(j + 1);
                tmp[j] += 1;
                if (tmp > dp[i + steps[j]])
                {
                    dp[i + steps[j]] = tmp;
                    frm[i + steps[j]] = i;
                }
                tmp[j] -= 1;
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
