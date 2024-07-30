#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

int main()
{
    ios::sync_with_stdio(0), cin.tie(0);
    int m, n, t, k, r;
    cin >> m >> n >> t >> k >> r;
    vector<int> arr(t);
    for (int &i : arr)
        cin >> i;
    vector<pair<int, int>> event;
    for (int i : arr)
    {
        event.emplace_back(i, 1);
        event.emplace_back(i + k, -1);
    }
    event.emplace_back(n + 1, 0);
    sort(event.begin(), event.end());
    long long ans = 0;
    int cnt = 0;
    for (int cur = k, i = 0; cur <= n;)
    {
        while (event[i].first <= cur)
            cnt += event[i++].second;
        int nxt = event[i].first;
        if (cnt & 1)
        {
            ans += (long long)(nxt - cur) * (min(m + 1, r + k) - max(k, r));
        }
        cur = nxt;
    }

    if (k & 1) ans = (long long)(n + 1 - k) * (m + 1 - k) - ans; 
    
    cout << ans << "\n";
}
