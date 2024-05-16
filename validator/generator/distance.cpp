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
Edges uniform_tree(int size, int &root, int base = 0) {
    if (size <= 0)
        __testlib_fail("uniform_tree: size must greater then 0");
    if (size == 1)
    {
        root = 0;
        return {};
    }
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
    root = rnd.next(0, size - 1); 
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
Edges custom_tree(int size, int &root, int type, int dis = 1, int base = 0) {
    if (size <= 0)
        __testlib_fail("custom_tree: size must greater then 0");
    if ((type == 1 || type == 2) && dis <= 0)
        __testlib_fail("custom_tree: dis must greater then 0 when type is 1 or 2");
    if (size == 1)
    {
        root = 0;
        return {};
    }
    Edges rt;
    std::vector<int> idx(size);
    std::iota(idx.begin(), idx.end(), 0);
    shuffle(idx.begin(), idx.end());
    root = idx[0];
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

std::vector<int> parse_tree(Edges &tree, int root) 
{
    int n = tree.size() + 1;
    std::vector<int> father(n), vis(n);
    std::vector<std::vector<int>> G(n);
    for (auto [u, v] : tree) 
    {
        G[u].push_back(v);
        G[v].push_back(u);
    }
    auto dfs = [&](auto _dfs, int u, int f) -> void 
    {
        father[u] = f;
        assert(!vis[u]);
        vis[u] = 1;
        for (int i : G[u])
            if (i != f)
                _dfs(_dfs, i, u);
    };
    dfs(dfs, root, -1);
    return father;
}

Edges perfect_binary_tree(int size, int &root)
{
    Edges res;
    root = 0;
    for (int i = 0; i < size; i++)
    {
        if (i * 2 + 1 < size) res.emplace_back(i, i * 2 + 1);
        if (i * 2 + 2 < size) res.emplace_back(i, i * 2 + 2);
    }
    return res;
}

void create_pair(int size, int &root, int base, Edges &tree, const std::vector<int> &param) // param = {kupper, bl, br, mxp}
{
    if (size == 1) 
    {
        root = base++;
        return;
    }
    int bl = param[1], br = param[2];
    br = std::min(size - 1, br);
    bl = std::min(bl, br);
    int b = rnd.next(bl, br);
    int kupper = param[0];
    kupper = std::min(kupper, (size - 1) / b);
    int k = rnd.wnext(1, kupper, param[3]);
   
    root = base++;
    auto branch = partition(b, size - 1 - b * k, 0);
    for (int i = 0; i < b; i++)
    {
        int cur = root;
        for (int j = 1; j < k; j++)
        {
            int nxt = base++;
            tree.emplace_back(cur, nxt);
            cur = nxt;
        }
        if (branch[i] > 0) 
        {
            int nxt;
            create_pair(branch[i] + 1, nxt, base, tree, param);
            tree.emplace_back(cur, nxt);
            base += branch[i] + 1;
        }
        else
            tree.emplace_back(cur, base++);
    }
}

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

void gen(int argc, char *argv[]) // n k type1 param1 type2 param2 reroot_flag
{ 
    assert(argc >= 7);
    int n = atoi(argv[0]);
    int k = atoi(argv[1]);
    int type[2] = {atoi(argv[2]), atoi(argv[4])};
    std::string param[2] = {argv[3], argv[5]};
    int reroot_flag = atoi(argv[6]);

    printf("%d %d\n", n, k);
    Edges tree[2];
    for (int i = 0; i < 2; i++) 
    {
        int root;
        if (type[i] == -1) tree[i] = uniform_tree(n, root);
        else if (type[i] <= 2) tree[i] = custom_tree(n, root, type[i], stoi(param[i]));
        else if (type[i] == 3) tree[i] = perfect_binary_tree(n, root);
        else if (type[i] == 4) create_pair(n, root, 0, tree[i], split_int(param[i]));
        else assert(0);
        shuffle_edges(tree[i]);
        if (reroot_flag >> i & 1)
            root = rnd.next(0, n - 1);
        std::vector<int> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        shuffle(idx.begin(), idx.end());
        root = idx[root];
        for (auto &[u, v] : tree[i])
            u = idx[u], v = idx[v];
        auto father = parse_tree(tree[i], root);
        for (int j = 0; j < n; j++)
        {
            printf("%d%c", father[j] + 1, " \n"[j + 1 == n]);
        }
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

