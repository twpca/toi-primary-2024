#include "testlib.h"
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

struct Disjoint_set
{
    int n;
    std::vector<int> boss;
    Disjoint_set(int _n): n(_n), boss(n) 
    {
        std::iota(boss.begin(), boss.end(), 0);
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

void validate(InStream &is, int n, std::vector<std::pair<int, int>> edges, const int &tnum)
{
    const std::string test_name = "testcase " + std::to_string(tnum) + ": ";
    std::vector<int> clr(n, -1);
    for (int i = 0; i < 3; i++)
    {
        int k = is.readInt(1, n, test_name + "k_" + std::to_string(i));
        for (int j = 0; j < k; j++)
        {
            int x = is.readInt(1, n, test_name + "c_" + std::to_string(i) + "," + std::to_string(j));
            if (clr[x - 1] != -1) 
            {
                quitf(TResult::_wa, "%sduplicate assignment: %d", test_name.c_str(), x);
            }
            clr[x - 1] = i;
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (clr[i] == -1)
        {
            quitf(TResult::_wa, "%smissing assignment: %d", test_name.c_str(), i + 1);
        }
    }
    Disjoint_set djs(n);
    for (auto [u, v] : edges)
    {
        u -= 1, v -= 1;
        if (clr[u] == clr[v]) continue;
        if (!djs.Union(u, v))
        {
            quitf(TResult::_wa, "%sdetected a cycle", test_name.c_str());
        }
    }
}

int main(int argc, char *argv[])
{
    inf.maxFileSize = 1024 * 1024 * 1024; // 1024MB
    setName("habitat_distribution special judge");
    registerTestlibCmd(argc, argv);

    /*
    Assume that the input is validated by the validator.
    Use invalid input may cause UB.
    */
    int t = inf.readInt();
    for (int i = 1; i <= t; i++)
    {
        int n = inf.readInt();
        int m = inf.readInt();
        std::vector<std::pair<int, int>> edges(m);
        for (auto &[u, v] : edges)
        {
            u = inf.readInt();
            v = inf.readInt();
        }
        validate(ouf, n, edges, i);
    }
    quitf(TResult::_ok, "contestant passed %d testcase(s)", t);
}
