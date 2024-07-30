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
    if (n > 1000 || m > 1000) return 0;
    vector<vector<int>> arr(m, vector<int>(n, 1));
    while (t--) 
    {
        int x;
        cin >> x;
        arr[r - 1][x - 1] = 0;
    }
    vector<pair<int, int>> event;
   
    int ans = 0;

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            if (i) arr[i][j] += arr[i - 1][j];
            if (j) arr[i][j] += arr[i][j - 1];
            if (i && j) arr[i][j] -= arr[i - 1][j - 1];
        }

    for (int i = k - 1; i < m; i++)
        for (int j = k - 1; j < n; j++)
        {
            int cur = arr[i][j];
            if (i >= k) cur -= arr[i - k][j];
            if (j >= k) cur -= arr[i][j - k];
            if (i >= k && j >= k) cur += arr[i - k][j - k];
            if (cur & 1) ans += 1;
        }

    cout << ans << "\n";
}
