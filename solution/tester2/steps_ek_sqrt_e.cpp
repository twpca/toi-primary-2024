#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>
using i08 = std::int8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
struct cost_t
{
    std::map<i32, i32> step_to_counts;
    i32 last_step;
    /*
    l  < r => 1
    l == r => 0
    l  > r => -1
    time complexity: O(sqrt(e) + log k)
    */
    int three_way(const cost_t &o) const
    {
        auto lit = step_to_counts.rbegin();
        auto led = step_to_counts.rend();
        auto rit = o.step_to_counts.rbegin();
        auto red = o.step_to_counts.rend();
        for (; lit != led && rit != red; ++lit, ++rit)
        {
            if (*lit < *rit)
            {
                return 1;
            }
            else if (*lit > *rit)
            {
                return -1;
            }
        }
        return 0;
    }
    bool operator<(const cost_t &o) const { return three_way(o) == 1; }
    bool operator==(const cost_t &o) const
    {
        return step_to_counts == o.step_to_counts;
    }
};

struct scope_t
{
    size_t k;
    size_t e;
    size_t E;
    std::vector<i08> holes;
    std::vector<i32> steps;
    bool set(std::istream &is)
    {
        size_t n;
        if (!(is >> n >> k >> e))
        {
            return false;
        }
        E = e + 1;
        holes.clear();
        holes.resize(E);
        for (size_t i = 0; i < n; ++i)
        {
            i32 hole{};
            is >> hole;
            holes[hole] = 1;
        }
        steps.resize(k);
        for (auto &step : steps)
        {
            is >> step;
        }
        std::sort(steps.begin(), steps.end());
        return bool(is);
    }
    void solve(std::ostream &os) const
    {
        const auto inf = cost_t{{std::pair(-1, 1)}, -1};
        std::vector<cost_t> min_cost(E, inf);
        min_cost[0] = cost_t{};
        for (size_t cur_pos = 0; cur_pos + steps[0] < E; ++cur_pos)
        {
            auto &cur_cost = min_cost[cur_pos];
            if (holes[cur_pos] || cur_cost == inf)
            {
                continue;
            }
            // std::cerr << "visited position: " << cur_pos << "\n";
            for (const auto &step : steps)
            {
                if (cur_pos + step >= E)
                {
                    break;
                }
                if (holes[cur_pos + step])
                {
                    continue;
                }
                auto new_cost = cost_t{
                    cur_cost.step_to_counts,
                    step,
                };
                new_cost.step_to_counts[step]++;
                auto &target_cost = min_cost[cur_pos + step];
                target_cost = std::max(new_cost, target_cost);
            }
        }
        if (min_cost[e] == inf)
        {
            os << -1 << '\n';
        }
        else
        {
            std::vector<size_t> ans;
            size_t cur_pos = e;
            for (; E > cur_pos && cur_pos > 0;)
            {
                auto &now = min_cost[cur_pos];
                ans.push_back(cur_pos);
                cur_pos -= now.last_step;
            }
            os << ans.size() << '\n';
            for (auto it = ans.rbegin(); it < ans.rend(); ++it)
            {
                os << *it << (it + 1 == ans.rend() ? '\n' : ' ');
            }
        }
    }
};

void init_io()
{
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
}

int main()
{
    init_io();
    for (scope_t scope{}; scope.set(std::cin);)
    {
        scope.solve(std::cout);
        std::cout.flush();
    }
}
