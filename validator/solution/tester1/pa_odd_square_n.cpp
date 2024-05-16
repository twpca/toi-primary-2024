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
    if (n > 100000) return 0;
    vector<int> arr(n, 0);
    while (t--)
    {
        int x;
        cin >> x;
        arr[x - 1] = 1;
    }
    long long ans = 0;
    int sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += arr[i];
        if (i >= k) sum -= arr[i - k];
        if (i >= k - 1 && sum & 1)
            ans += min(m + 1, r + k) - max(k, r);
    }

    if (k & 1) ans = (long long)(n + 1 - k) * (m + 1 - k) - ans; 
    
    cout << ans << "\n";
}
