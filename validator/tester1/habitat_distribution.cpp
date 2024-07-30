#include "testlib.h"
#include <algorithm>
#include <vector>
#include <set>
#include <utility>
#include <string>
#include <cassert>
#include <numeric>

constexpr int SUBTASKS = 4;
constexpr int MN_T = 1;
constexpr int MX_T = 300000;
constexpr int MN_N = 3;
constexpr int MX_N[SUBTASKS + 1] = {300000, 300000, 300000, 500, 300000};

int subtask(int argc, char* argv[]) 
{
    for (int i = 1; i < argc; i++) 
    {
        if (!strcmp("--testcase", argv[i - 1])) 
        {
            int subtask = atoi(argv[i]);
            assert(0 <= subtask && subtask <= SUBTASKS);
            return subtask;
        }
    }
    assert(0);
    return -1;
}

struct Disjoint_set 
{
    int n;
    std::vector<int> boss;
    Disjoint_set(int _n = 0): n(_n), boss(n + 1) 
    {
        std::iota(boss.begin(), boss.end(), 0);
    }
    int finds(int x) 
    {
        if (x == boss[x]) return x;
        return boss[x] = finds(boss[x]);
    }
    bool Union(int a, int b) 
    {
        a = finds(a), b = finds(b);
        if (a == b) return false;
        boss[a] = b;
        return true;
    }
};

int main(int argc, char* argv[]) 
{
    registerValidation(argc, argv);
    int s = subtask(argc, argv);
    int t = inf.readInt(MN_T, MX_T, "t"); inf.readChar('\n');
    int sum_n = 0;

    for (int i = 1; i <= t; i++)
    {
        const std::string test_name = "testcase " + std::to_string(i) + ": ";
        int n = inf.readInt(MN_N, MX_N[s], test_name + "n"); inf.readSpace();
        int m = inf.readInt(0, n * 2 - 4, test_name + "t"); inf.readChar('\n');
        sum_n += n;
        ensuref(sum_n <= MX_N[s], "%ssum of n exceed limit %d", test_name.c_str(), MX_N[s]);
        std::set<std::pair<int, int>> vis;
        if (s == 1) ensuref(m == n - 1, "%sm != n - 1", test_name.c_str());
        Disjoint_set djs(n);
        int conn = n;
        for (int j = 0; j < m; j++)
        {
            int u = inf.readInt(1, n, test_name + "u_" + std::to_string(j)); inf.readSpace(); 
            int v = inf.readInt(1, n, test_name + "v_" + std::to_string(j)); inf.readChar('\n');
            if (u > v) std::swap(u, v);
            ensuref(vis.emplace(u, v).second, "%smultiple edges", test_name.c_str());
            ensuref(u != v, "%sloop", test_name.c_str());
            conn -= djs.Union(u - 1, v - 1);
        }
        if (s == 1) ensuref(conn == 1, "%snot connected", test_name.c_str());
        else if (s == 2) ensuref(conn > 1, "%sconnected", test_name.c_str());
    }

    inf.readEof();
    return 0;
}
