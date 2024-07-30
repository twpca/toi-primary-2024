#include <algorithm>
#include <cstdint>
#include <iostream>
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
    i32 max_step_counts;
    i32 last_step;
    [[nodiscard]] auto key() const { return std::tie(max_step_counts); }
    bool operator==(const cost_t &rhs) const { return key() == rhs.key(); }
    bool operator<(const cost_t &rhs) const { return key() < rhs.key(); }
};

struct scope_t
{
    size_t n;
    size_t k;
    size_t e;
    size_t E;
    std::vector<i08> holes;
    std::vector<i32> steps;
    bool set(std::istream &ins)
    {
        if (!(ins >> n >> k >> e))
        {
            return false;
        }
        E = e + 1;
        holes.clear();
        holes.resize(E);
        for (size_t i = 0; i < n; ++i)
        {
            i32 hole{};
            ins >> hole;
            holes[hole] = 1;
        }
        steps.resize(k);
        for (auto &step : steps)
        {
            ins >> step;
        }
        std::sort(steps.begin(), steps.end(), std::greater());
        return bool(ins);
    }
    void solve(std::ostream &ous) const
    {
        const auto inf = cost_t{-1};
        std::vector<cost_t> min_cost(E, inf);
        min_cost[0] = cost_t{0};
        const auto min_step = steps.back();
        for (size_t cur_pos = 0; cur_pos + min_step < E; ++cur_pos)
        {
            auto &cur_cost = min_cost[cur_pos];
            if (static_cast<bool>(holes[cur_pos]) || cur_cost == inf)
            {
                continue;
            }
            // std::cerr << "visited position: " << cur_pos << "\n";
            for (const auto &step : steps)
            {
                if (cur_pos + step >= E)
                {
                    continue;
                }
                if (static_cast<bool>(holes[cur_pos + step]))
                {
                    continue;
                }
                auto new_cost = cost_t{
                    cur_cost.max_step_counts +
                        static_cast<int>(step == steps.front()),
                    step,
                };
                auto &target_cost = min_cost[cur_pos + step];
                target_cost = std::max(new_cost, target_cost);
            }
        }
        if (min_cost[e] == inf)
        {
            ous << -1 << '\n';
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
            ous << ans.size() << '\n';
            for (auto it = ans.rbegin(); it < ans.rend(); ++it)
            {
                ous << *it << (it + 1 == ans.rend() ? '\n' : ' ');
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
