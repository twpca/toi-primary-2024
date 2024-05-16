#include "testlib.h"
#include <bits/stdc++.h>

typedef std::pair<int, int> Edge;
typedef std::vector<Edge> Edges;

/*
O(n)
*/
void shuffle_edges(Edges &edges, int base = 0) {
    shuffle(edges.begin(), edges.end());
    for (auto &e : edges) {
        if (rnd.next(0, 1))
            std::swap(e.first, e.second);
        e.first += base;
        e.second += base;
    }
}

/*
O(nlogn)
*/
void relabel_edges(Edges &edges, int base = 0) {
    std::vector<int> label;
    for (auto &e : edges) {
        label.push_back(e.first);
        label.push_back(e.second);
    }
    std::sort(label.begin(), label.end());
    label.resize(std::unique(label.begin(), label.end()) - label.begin());
    for (auto &e : edges) {
        e.first = std::lower_bound(label.begin(), label.end(), e.first) - label.begin() + base;
        e.second = std::lower_bound(label.begin(), label.end(), e.second) - label.begin() + base;
    }
}

/*
convert from Prüfer sequence
O(nlogn)
*/
Edges uniform_tree(int size, int base = 0) {
    if (size <= 0)
        __testlib_fail("uniform_tree: size must greater then 0");
    if (size == 1)
        return {};
    Edges rt;
    std::vector<int> seq(size - 2), cnt(size, 0);
    std::priority_queue<int, std::vector<int>, std::greater<int>> leaves;
    Edge lst;
    for (int i = 0; i < size - 2; ++i) {
        seq[i] = rnd.next(0, size - 1);
        ++cnt[seq[i]];
    }
    for (int i = 0; i < size; ++i)
        if (!cnt[i])
            leaves.push(i);
    for (int i : seq) {
        rt.push_back(std::make_pair(leaves.top(), i));
        leaves.pop();
        if (!--cnt[i])
            leaves.push(i);
    }
    lst.first = leaves.top();
    leaves.pop();
    lst.second = leaves.top();
    rt.push_back(lst);
    shuffle_edges(rt, base);
    return rt;
}

/*
shuffle the nodes and connect one from prefix for each node, for each node:
type0: no restriction
type1: connect with rnd.next(std::max(0, i - dis), i - 1)
type2: connect with rnd.next(0, std::min(dis - 1, i - 1))
Note that when type not equal to 1 and 2, it is regraded as 0
O(n)
*/
Edges custom_tree(int size, int type, int dis = 1, int base = 0) {
    if (size <= 0)
        __testlib_fail("custom_tree: size must greater then 0");
    if ((type == 1 || type == 2) && dis <= 0)
        __testlib_fail("custom_tree: dis must greater then 0 when type is 1 or 2");
    if (size == 1)
        return {};
    Edges rt;
    std::vector<int> idx(size);
    std::iota(idx.begin(), idx.end(), 0);
    shuffle(idx.begin(), idx.end());
    for (int i = 1; i < size; ++i)
        if (type == 1)
            rt.push_back(std::make_pair(idx[rnd.next(std::max(0, i - dis), i - 1)], idx[i]));
        else if (type == 2)
            rt.push_back(std::make_pair(idx[rnd.next(0, std::min(dis - 1, i - 1))], idx[i]));
        else
            rt.push_back(std::make_pair(idx[rnd.next(0, i - 1)], idx[i]));
    shuffle_edges(rt, base);
    return rt;
}

/* Returns random permutation of the given size (values are between `first` and `first`+size-1)*/
template<typename T, typename E>
std::vector<E> perm(T size, E first) {
    if (size < 0)
        __testlib_fail("random_t::perm(T size, E first = 0): size must non-negative");
    else if (size == 0)
        return std::vector<E>();
    std::vector<E> p(size);
    E current = first;
    for (T i = 0; i < size; i++)
        p[i] = current++;
    if (size > 1)
        for (T i = 1; i < size; i++)
            std::swap(p[i], p[rnd.next(i + 1)]);
    return p;
}

/* Returns random permutation of the given size (values are between 0 and size-1)*/
template<typename T>
std::vector<T> perm(T size) {
    return perm(size, T(0));
}

/* Returns `size` unordered (unsorted) distinct numbers between `from` and `to`. */
template<typename T>
std::vector<T> distinct(int size, T from, T to) {
    std::vector<T> result;
    if (size == 0)
        return result;

    if (from > to)
        __testlib_fail("random_t::distinct expected from <= to");

    if (size < 0)
        __testlib_fail("random_t::distinct expected size >= 0");

    uint64_t n = to - from + 1;
    if (uint64_t(size) > n)
        __testlib_fail("random_t::distinct expected size <= to - from + 1");

    double expected = 0.0;
    for (int i = 1; i <= size; i++)
        expected += double(n) / double(n - i + 1);

    if (expected < double(n)) {
        std::set<T> vals;
        while (int(vals.size()) < size) {
            T x = T(rnd.next(from, to));
            if (vals.insert(x).second)
                result.push_back(x);
        }
    } else {
        if (n > 1000000000)
            __testlib_fail("random_t::distinct here expected to - from + 1 <= 1000000000");
        std::vector<T> p(perm(int(n), from));
        result.insert(result.end(), p.begin(), p.begin() + size);
    }

    return result;
}

/* Returns `size` unordered (unsorted) distinct numbers between `0` and `upper`-1. */
template<typename T>
std::vector<T> distinct(int size, T upper) {
    if (size < 0)
        __testlib_fail("random_t::distinct expected size >= 0");
    if (size == 0)
        return std::vector<T>();

    if (upper <= 0)
        __testlib_fail("random_t::distinct expected upper > 0");
    if (size > upper)
        __testlib_fail("random_t::distinct expected size <= upper");

    return distinct(size, T(0), upper - 1);
}

/* Returns random (unsorted) partition which is a representation of sum as a sum of integers not less than min_part. */
template<typename T>
std::vector<T> partition(int size, T sum, T min_part) {
    if (size < 0)
        __testlib_fail("random_t::partition: size < 0");
    if (size == 0 && sum != 0)
        __testlib_fail("random_t::partition: size == 0 && sum != 0");
    if (min_part * size > sum)
        __testlib_fail("random_t::partition: min_part * size > sum");
    if (size == 0 && sum == 0)
        return std::vector<T>();

    T sum_ = sum;
    sum -= min_part * size;

    std::vector<T> septums(size);
    std::vector<T> d = distinct(size - 1, T(1), T(sum + size - 1));
    for (int i = 0; i + 1 < size; i++)
        septums[i + 1] = d[i];
    std::sort(septums.begin(), septums.end());

    std::vector<T> result(size);
    for (int i = 0; i + 1 < size; i++)
        result[i] = septums[i + 1] - septums[i] - 1;
    result[size - 1] = sum + size - 1 - septums.back();

    for (std::size_t i = 0; i < result.size(); i++)
        result[i] += min_part;

    T result_sum = 0;
    for (std::size_t i = 0; i < result.size(); i++)
        result_sum += result[i];
    if (result_sum != sum_)
        __testlib_fail("random_t::partition: partition sum is expected to be the given sum");

    if (*std::min_element(result.begin(), result.end()) < min_part)
        __testlib_fail("random_t::partition: partition min is expected to be no less than the given min_part");

    if (int(result.size()) != size || result.size() != (size_t) size)
        __testlib_fail("random_t::partition: partition size is expected to be equal to the given size");

    return result;
}

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

std::vector<int> split_int(std::string str) 
{
    for (char &c : str)
        if (c == ',')
            c = ' ';
    std::stringstream ss(str);
    std::vector<int> res;
    for (int x; ss >> x; res.push_back(x));
    return res;
}

void shuffle_idx(int n, Edges &edges, int offset = 0)
{
    std::vector<int> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    shuffle(idx.begin() + offset, idx.end());
    for (auto &[u, v] : edges)
    {
        u = idx[u];
        v = idx[v];
    }
}

void random_addition(int l, int r, int target_size, Edges &edges)
{
    std::set<std::pair<int, int>> vis;
    target_size = std::min((long long)target_size, (long long)(r - l + 1) * (r - l) / 2);
    for (auto [u, v] : edges)
    {
        if (u > v) std::swap(u, v);
        assert(vis.emplace(u, v).second);
    }
    while (int(vis.size()) < target_size)
    {
        int u = rnd.next(l, r);
        int v = rnd.next(l, r);
        if (u == v) continue;
        if (vis.emplace(std::min(u, v), std::max(u, v)).second)
            edges.emplace_back(u, v);
    }
}

Edges pure_random(int &n, int type) // 0
{
    Edges res;
    random_addition(0, n - 1, 2 * n - 4, res);
    return res;
}

Edges complete_bigraph(int &n, int type) // 1
{
    Edges res;
    for (int i = 2; i < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges twoK4(int &n, int type) // 2
{
    Edges res;
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++)
        {
            res.emplace_back(i, j);
        }
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++)
        {
            res.emplace_back(i + 4, j + 4);
        }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges clique_flower(int &n, int type) // 3, 4, 5
{
    Edges res;

    auto add2 = [&](int &base)
    {
        res.emplace_back(0, base);
        res.emplace_back(0, base + 1);
        base += 2;
    };

    auto add3 = [&](int &base)
    {
        res.emplace_back(0, base);
        res.emplace_back(0, base + 1);
        res.emplace_back(0, base + 2);
        res.emplace_back(base, base + 1);
        res.emplace_back(base, base + 2);
        res.emplace_back(base + 1, base + 2);
        base += 3;
    };

    auto add4 = [&](int &base)
    {
        res.emplace_back(0, base);
        res.emplace_back(0, base + 1);
        res.emplace_back(0, base + 2);
        res.emplace_back(0, base + 3);
        res.emplace_back(base, base + 1);
        res.emplace_back(base, base + 2);
        res.emplace_back(base, base + 3);
        res.emplace_back(base + 1, base + 2);
        res.emplace_back(base + 1, base + 3);
        res.emplace_back(base + 2, base + 3);
        base += 4;
    };

    if (n == 3)
    {
        res.emplace_back(0, 1); 
        res.emplace_back(0, 2); 
    }
    else if (type == 3 || n <= 7)
    {
        int base = 1;
        if (n % 2 == 0) 
        {
            res.emplace_back(0, 1);
            res.emplace_back(0, 2);
            res.emplace_back(0, 3);
            res.emplace_back(1, 2);
            if (n > 4)
            {
                res.emplace_back(1, 3);
                res.emplace_back(2, 3);
            }
            base += 3;
        }
        while (base < n)
        {
            add2(base);
        }
    }
    else if (type == 4)
    {
        int base = 1;
        add2(base);
        add2(base);
        while (base < n)
        {
            if (base + 2 == n) add2(base);
            else if (base + 3 == n) add3(base);
            else if (base + 4 == n) add2(base), add2(base);
            else add3(base);
        }
    }
    else if (type == 5)
    {
        int base = 1;
        add2(base);
        add2(base);
        while (base < n)
        {
            if (base + 2 == n) add2(base);
            else if (base + 3 == n) add3(base);
            else if (base + 4 == n) add2(base), add2(base);
            else if (base + 5 == n) add3(base);
            else if (base + 6 == n) add3(base);
            else if (base + 7 == n) add3(base);
            else if (base + 8 == n) add2(base), add2(base), add4(base);
            else if (base + 9 == n) add3(base);
            else if (rnd.next(0, 1)) add3(base);
            else add2(base), add2(base), add4(base);
        }
    }
    else assert(0);

    random_addition(0, n - 1, 2 * n - 4, res);
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges no_edge(int &n, int type) // 6
{
    return {};
}

Edges leaf_flower(int &n, int type) // 7
{
    if (n <= 7) return complete_bigraph(n, 1);

    Edges res;

    res.emplace_back(0, 1);
    res.emplace_back(0, 2);
    res.emplace_back(0, 3);
    res.emplace_back(1, 2);
    res.emplace_back(2, 3);

    res.emplace_back(1, 4);

    res.emplace_back(0, 4);
    res.emplace_back(0, 5);
    res.emplace_back(0, 6);
    res.emplace_back(4, 5);
    res.emplace_back(5, 6);

    res.emplace_back(0, 7);
    for (int i = 8; i < n; i++)
    {
        res.emplace_back(0, i);
        res.emplace_back(i - 1, i);
    }

    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges one_component_random(int &n, int type) // 8
{
    Edges res = uniform_tree(n);
    random_addition(0, n - 1, 2 * n - 4, res);
    shuffle_edges(res);
    return res;
}

Edges two_component_random(int &n, int type) // 9
{
    int n1 = rnd.next(1, n - 1);
    int n2 = n - n1;
    Edges G1 = uniform_tree(n1);
    Edges G2 = uniform_tree(n2, n1);
    int m = 2 * n - 4 - (n - 2);
    int m1 = rnd.next(0, m);
    int m2 = m - m1;
    random_addition(0, n1 - 1, m1 + n1 - 1, G1);
    random_addition(n1, n - 1, m2 + n2 - 1, G2);
    G1.insert(G1.end(), G2.begin(), G2.end());
    shuffle_edges(G1);
    shuffle_idx(n, G1);
    return G1;
}

Edges gen_dfs_tree_hacker(int n, int base = 0) // not a generator
{
    if (n == 1) return {};
    if (n == 2) return {{base, base + 1}};
    if (n == 3) return {{base, base + 1}, {base, base + 2}, {base + 1, base + 2}};
    int branch = rnd.next(1, (n - 1) / 3);
    Edges res;
    for (int i = 1; i <= branch; i++)
    {
        // tree edges
        res.emplace_back(0, i);
        res.emplace_back(i, i + branch);
        res.emplace_back(i + branch, i + branch + branch);

        // back edges
        res.emplace_back(0, i + branch);
        res.emplace_back(0, i + branch + branch);
        res.emplace_back(i, i + branch + branch);
    }
    for (int i = branch * 3 + 1; i < n; i++)
    {
        int r = rnd.next(1, i - 1);

        // tree edges
        res.emplace_back(r, i);

        // back edges
        res.emplace_back(0, i);
    }

    shuffle_idx(n, res, 1);
    for (auto &[u, v] : res)
        u += base, v += base;
    return res;
}

Edges two_component_single_dfs_tree_hacker(int &n, int type) // 10
{
    Edges res = gen_dfs_tree_hacker(n - 1);
    int r = rnd.next(0, n);
    for (auto &[u, v] : res)
    {
        if (u >= r) u += 1;
        if (v >= r) v += 1;
    }
    return res;
}

Edges dfs_tree_hacker(int &n, int type) // 11
{
    int n1 = rnd.next(1, n - 1);
    int n2 = n - n1;
    Edges G1 = gen_dfs_tree_hacker(n1);
    Edges G2 = gen_dfs_tree_hacker(n2, n1);
    int p = 0, q = 0;
    Edges res;
    while (p < int(G1.size()) || q < int(G2.size()))
    {
        if (q >= int(G2.size()) || (p < int(G1.size()) && rnd.next(0, 1)))
            res.push_back(G1[p++]);
        else
            res.push_back(G2[q++]);
    }
    return res;
}

Edges tree(int &n, int type) // 12
{
    return uniform_tree(n);  
}

Edges path(int &n, int type) // 13
{
    return custom_tree(n, 1, 1);
}

Edges diameter2_no_av(int &n, int type) // 14
{
    if (n < 7) return complete_bigraph(n, 1);
    Edges res;
    res.emplace_back(0, 1);
    res.emplace_back(0, 2);
    res.emplace_back(1, 3);
    res.emplace_back(1, 4);
    res.emplace_back(2, 5);
    res.emplace_back(2, 6);
    res.emplace_back(3, 4);
    res.emplace_back(3, 5);
    res.emplace_back(4, 6);
    res.emplace_back(5, 6);
    for (int i = 7; i < n; i++)
    {
        res.emplace_back(1, i);
        res.emplace_back(2, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges eight(int &n, int type) // 15
{
    Edges res;
    for (int i = 0; i < 8; i++)
        res.emplace_back(i, (i + 1) % 8);
    for (int i = 0; i < 4; i++)
        res.emplace_back(i, i + 4);
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges regular3(int &n, int type) // 16
{
    if (n <= 7) return complete_bigraph(n, 1);
    if (n & 1) n -= 1;
    Edges res;
    for (int i = 0; i < n; i++)
        res.emplace_back(i, (i + 1) % n);
    for (int i = 0; i < n / 2; i++)
        res.emplace_back(i, i + n / 2);
    random_addition(0, n - 1, 2 * n - 4, res);
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges gen_complete_bigraph_add_two_edges(int n) // not a generator
{
    Edges res;
    for (int i = 2; i < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    if (n >= 2)
        res.emplace_back(0, 1);
    if (n >= 4)
        res.emplace_back(2, 3);
    return res;
}

Edges small_large_complete_bigraph(int &n, int type) // 17
{
    int n1 = rnd.next(1, std::min(n - 1, 6));
    int n2 = n - n1;
    Edges G1 = gen_complete_bigraph_add_two_edges(n1);
    Edges G2 = gen_complete_bigraph_add_two_edges(n2);
    for (auto &[u, v] : G2)
        u += n1, v += n1;
    G1.insert(G1.end(), G2.begin(), G2.end());
    shuffle_edges(G1);
    shuffle_idx(n, G1);
    return G1;
}

Edges complete_bigraph_with_a_leaf(int &n, int type) // 18
{
    Edges res;
    for (int i = 2; i + 1 < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    res.emplace_back(0, n - 1);
    if (n > 2)
        res.emplace_back(0, 1);
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges multi_complete_bigraph(int &n, int type) // 19
{
    Edges res;
    auto num = partition(rnd.next(3, std::min(n, 6)), n, 1);
    auto addition = partition(int(num.size()), 2 * n - 4 - (n - 2 * int(num.size())), 0);
    int sum = 0;
    for (int i = 0; i < int(num.size()); i++)
    {
        auto G = gen_complete_bigraph_add_two_edges(num[i]);
        int tar_size = G.size() + addition[i];
        tar_size = std::min((long long)tar_size, (long long)num[i] * (num[i] - 1) / 2);
        random_addition(0, num[i] - 1, tar_size, G);
        for (auto &[u, v] : G)
            u += sum, v += sum;
        res.insert(res.end(), G.begin(), G.end());
        sum += num[i];
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges complete_bigraph_with_a_flag(int &n, int type) // 20
{
    if (n < 6) return complete_bigraph_with_a_leaf(n, 18);
    Edges res;
    res.emplace_back(0, 1);
    res.emplace_back(0, 4);
    res.emplace_back(0, 5);
    res.emplace_back(1, 2);
    res.emplace_back(1, 3);
    res.emplace_back(1, 4);
    res.emplace_back(2, 3);
    res.emplace_back(4, 5);
    for (int i = 6; i < n; i++) 
    {
        res.emplace_back(4, i);
        res.emplace_back(5, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges L3tri(int &n, int type) // 21
{
    if (n < 5) return complete_bigraph_with_a_leaf(n, 18);
    n = (n - 2) / 3 * 3 + 2;
    Edges res;
    res.emplace_back(0, 1);
    res.emplace_back(0, 2);
    res.emplace_back(1, 3);
    res.emplace_back(1, 4);
    res.emplace_back(2, 3);
    res.emplace_back(3, 4);
    for (int i = 5; i + 3 <= n; i += 3) 
    {
        std::vector<int> idx({2, 3, 4});
        shuffle(idx.begin(), idx.end());
        res.emplace_back(i, i + 1);
        res.emplace_back(i, i + 2);
        res.emplace_back(i + 1, i + 2);
        for (int j = 0; j < 3; ++j)
            res.emplace_back(idx[j], i + j);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges complete_bigraph_with_a_triangle(int &n, int type) // 22
{
    if (n < 4) return complete_bigraph(n, 1);
    Edges res;
    res.emplace_back(0, 2);
    res.emplace_back(0, 3);
    res.emplace_back(2, 3);
    res.emplace_back(0, 1);
    for (int i = 4; i + 1 < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges complete_bigraph_with_a_square(int &n, int type) // 23
{
    if (n < 4) return complete_bigraph(n, 1);
    Edges res;
    res.emplace_back(0, 2);
    res.emplace_back(1, 3);
    res.emplace_back(2, 3);
    res.emplace_back(0, 1);
    for (int i = 4; i + 1 < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges complete_bigraph_with_a_boat(int &n, int type) // 24
{
    if (n < 6) return complete_bigraph(n, 1);
    Edges res;
    res.emplace_back(0, 2);
    res.emplace_back(1, 3);
    res.emplace_back(2, 3);
    res.emplace_back(0, 1);
    res.emplace_back(0, 4);
    res.emplace_back(2, 4);
    res.emplace_back(1, 5);
    res.emplace_back(3, 5);
    for (int i = 6; i + 1 < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges complete_bigraph_with_a_pavilion(int &n, int type) // 25
{
    if (n < 6) return complete_bigraph(n, 1);
    Edges res;
    res.emplace_back(0, 2);
    res.emplace_back(1, 3);
    res.emplace_back(2, 3);
    res.emplace_back(0, 1);
    res.emplace_back(0, 4);
    res.emplace_back(3, 4);
    res.emplace_back(1, 5);
    res.emplace_back(2, 5);
    for (int i = 6; i + 1 < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges complete_bigraph_with_eight(int &n, int type) // 26
{
    if (n < 8) return complete_bigraph(n, 1);
    Edges res;
    for (int i = 0; i < 8; i++)
        res.emplace_back(i, (i + 1) % 8);
    for (int i = 0; i < 4; i++)
        res.emplace_back(i, i + 4);
    for (int i = 8; i + 1 < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

Edges complete_bigraph_with_eight2(int &n, int type) // 27
{
    if (n < 8) return complete_bigraph(n, 1);
    Edges res;
    for (int i = 0; i < 8; i++)
        res.emplace_back(i, (i + 1) % 8);
    res.emplace_back(0, 2);
    res.emplace_back(3, 7);
    res.emplace_back(4, 6);
    res.emplace_back(1, 5);
    for (int i = 8; i + 1 < n; i++) 
    {
        res.emplace_back(0, i);
        res.emplace_back(1, i);
    }
    shuffle_edges(res);
    shuffle_idx(n, res);
    return res;
}

void gen(int argc, char *argv[]) // t, sum, min_part, type
{ 
    std::map<int, int> fixed_size;
    fixed_size[2] = 8;
    fixed_size[15] = 8;
    std::vector<std::function<Edges(int&, int)>> generator;

    generator.push_back(pure_random);
    generator.push_back(complete_bigraph);
    generator.push_back(twoK4);
    generator.push_back(clique_flower);
    generator.push_back(clique_flower);
    generator.push_back(clique_flower);
    generator.push_back(no_edge);
    generator.push_back(leaf_flower);
    generator.push_back(one_component_random);
    generator.push_back(two_component_random);
    generator.push_back(two_component_single_dfs_tree_hacker);
    generator.push_back(dfs_tree_hacker);
    generator.push_back(tree);
    generator.push_back(path);
    generator.push_back(diameter2_no_av);
    generator.push_back(eight);
    generator.push_back(regular3);
    generator.push_back(small_large_complete_bigraph);
    generator.push_back(complete_bigraph_with_a_leaf);
    generator.push_back(multi_complete_bigraph);
    generator.push_back(complete_bigraph_with_a_flag);
    generator.push_back(L3tri);
    generator.push_back(complete_bigraph_with_a_triangle);
    generator.push_back(complete_bigraph_with_a_square);
    generator.push_back(complete_bigraph_with_a_boat);
    generator.push_back(complete_bigraph_with_a_pavilion);
    generator.push_back(complete_bigraph_with_eight);
    generator.push_back(complete_bigraph_with_eight2);

    assert(argc >= 4);
    int t = atoi(argv[0]);
    int sum = atoi(argv[1]);
    int min_part = atoi(argv[2]);
    std::vector<int> types = split_int(argv[3]);
    std::vector<std::pair<int, int>> testcase(t);
    int req = 0;
    for (auto &[sz, type] : testcase)
    {
        type = types[rnd.next(0, int(types.size()) - 1)];
        if (fixed_size.find(type) != fixed_size.end())
        {
            sz = fixed_size[type];
            sum -= sz;
        }
        else
            req += 1;
    }
    assert(sum >= req * min_part);
    auto req_size = req ? partition(req, sum, min_part) : std::vector<int>();
    printf("%d\n", t);
    for (auto &[sz, type] : testcase)
    {
        if (sz == 0)
        {
            sz = req_size.back();
            req_size.pop_back();
        }
        Edges edges = generator[type](sz, type);
        printf("%d %d\n", sz, int(edges.size()));
        for (auto [u, v] : edges)
            printf("%d %d\n", u + 1, v + 1);
    }
}

#define register_generator(name) if (gname == #name) return name(argc - 2, argv + 2), 0

int main(int argc, char* argv[])
{
    if (argc < 2)
    {   
        return std::cerr << "Arguments should not be empty.\n", 1;
    }
    registerGen(argc, argv, 1);
    std::string gname(argv[1]);
    register_generator(gen);
    return 0;
}
