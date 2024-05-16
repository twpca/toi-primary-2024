#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    int n, d;
    cin >> n >> d;
    while (n--)
    {
        long long x;
        cin >> x;
        vector<long long> appear;
        while (true)
        {
            int flag = 0;
            for (auto p : appear)
                if (p == x)
                {
                    flag = 1;
                    break;
                }
            if (flag)
            {
                cout << x << "\n";
                break;
            }
            appear.push_back(x);
            vector<int> big, small;
            for (int i = 0; i < d; i++)
            {
                big.push_back(x % 10);
                x /= 10;
            }
            sort(big.begin(), big.end());
            small = big;
            reverse(big.begin(), big.end());
            long long big_num = 0, small_num = 0;
            for (int i = 0; i < d; i++)
            {
                big_num = big_num * 10 + big[i];
                small_num = small_num * 10 + small[i];
            }
            x = big_num - small_num;
        }
    }
}
