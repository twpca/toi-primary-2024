#include "testlib.h"
#include <algorithm>
#include <vector>
#include <set>
#include <utility>
#include <string>
#include <cassert>
#include <numeric>

constexpr int SUBTASKS = 5;
constexpr int MN_N = 1;
constexpr int MX_N[SUBTASKS + 1] = {200000, 100, 3000, 200000, 200000, 200000};
constexpr int MN_K = 0;
constexpr int MX_K[SUBTASKS + 1] = {200000, 100, 3000, 0, 20, 200000};

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
    int n = inf.readInt(MN_N, MX_N[s], "n"); inf.readSpace();
    int k = inf.readInt(MN_K, std::min(MX_K[s], n - 1), "k"); inf.readChar('\n');

    auto readTree = [&](const std::string &tree_name)
    {
        int conn = n, root = -1;
        Disjoint_set djs(n);

        for (int i = 1; i <= n; i++) 
        {
            int father = inf.readInt(0, n, tree_name + "_" + std::to_string(i)); 
            if (i == n) inf.readChar('\n');
            else inf.readSpace();

            if (father == 0)
            {
                ensuref(root == -1, "multiple roots");
                root = i;   
            }
            else
                conn -= djs.Union(i, father);
        }

        ensuref(conn == 1, "not connected");
    };

    readTree("p"), readTree("q");

    inf.readEof();
    return 0;
}
