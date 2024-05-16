#include "testlib.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
using i08 = std::int8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
constexpr int max_n = 1'000'000;
constexpr int max_k = 1'000'000;
constexpr int max_e = 1'000'000;
const double subtask_1_score = 0.2;
const double subtask_2_score = 0.5;

struct cost_t
{
    std::vector<i32> counts;
    [[nodiscard]] auto key() const { return std::tie(counts); }
    bool operator==(const cost_t &rhs) const { return key() == rhs.key(); }
    bool operator!=(const cost_t &rhs) const { return !operator==(rhs); }
    bool operator<(const cost_t &rhs) const { return key() < rhs.key(); }
};

const cost_t no_answer{};

auto count_and_validate(InStream &ins, const int input_e,
                        const std::vector<i08> &holes,
                        const std::map<i32, i32> &step_to_rank,
                        std::string_view name) -> cost_t
{
    const int user_input_m = ins.readInt(-1, input_e, "m");
    if (user_input_m == -1)
    {
        return no_answer;
    }
    int last = 0;
    cost_t cost{};
    cost.counts.resize(step_to_rank.size());
    for (int i = 0; i < user_input_m; ++i)
    {
        const auto var_name = std::to_string(i) + "th position";
        /* Assert range prevent user input underflow. */
        auto next = ins.readInt(1, input_e, var_name);
        const auto step_size = next - last;
        if (0 >= next || next >= static_cast<int>(holes.size()))
        {
            quitf(TResult::_wa, "%.*s: %dth position:%d is not in (%d, %d)",
                  static_cast<int>(name.size()), name.data(), i, next, 0,
                  int(holes.size()));
        }
        if (static_cast<bool>(holes.at(next)))
        {
            quitf(TResult::_wa, "%.*s: %dth position:%d is in a hole",
                  static_cast<int>(name.size()), name.data(), i, next);
        }
        auto itr = step_to_rank.find(step_size);
        if (itr == step_to_rank.end())
        {
            quitf(TResult::_wa,
                  "%.*s: distance between %dth position:%d and "
                  "%dth position:%d is %d and is not in available steps.",
                  static_cast<int>(name.size()), name.data(), i, next, i - 1,
                  last, step_size);
        }
        const auto &rank = itr->second;
        ++cost.counts.at(rank);
        last = next;
    }
    if (last != input_e)
    {
        quitf(TResult::_wa, "%.*s: last position is %d != %d",
              static_cast<int>(name.size()), name.data(), last, input_e);
    }
    return cost;
}

int main(int argc, char *argv[])
{
    constexpr size_t unit = 1024;
    inf.maxFileSize = unit * unit * unit; // 1024MB
    setName("steps special judge");
    registerTestlibCmd(argc, argv);

    /*
    read inputs
    Assume that the input is validated by the validator.
    Use invalid input may cause UB.
    Guard n, k, e for large memory allocation.
    */
    const int input_n = inf.readInt(0, max_n, "n");
    const int input_k = inf.readInt(0, max_k, "k");
    const int input_e = inf.readInt(0, max_e, "e");
    std::map<i32, i32> step_to_rank{};
    std::vector<i08> holes(input_e + 1);
    for (int i = 0; i < input_n; ++i)
    {
        auto val = inf.readInt(0, input_e);
        holes.at(val) = 1;
    }
    for (int i = 0; i < input_k; ++i)
    {
        step_to_rank.insert({inf.readInt(), -1});
    }
    {
        i32 rank = input_k - 1;
        for (auto &val : step_to_rank)
        {
            val.second = rank--;
        }
    }
    // read ans
    const auto standard_info =
        count_and_validate(ans, input_e, holes, step_to_rank, "standard");
    // read user output
    const auto user_info =
        count_and_validate(ouf, input_e, holes, step_to_rank, "user");

    if (standard_info == no_answer)
    {
        if (user_info == no_answer)
        {
            quitf(TResult::_ok, "The no answer case.");
        }
        else
        {
            quitf(TResult::_ok,
                  "Warning! No standard solution but user found a solution :O");
        }
    }

    if (user_info == no_answer)
    {
        quitf(TResult::_wa, "User gives no answer but there is an answer!");
    }
    // standard_info and user_info both contain answers
    auto result =
        std::mismatch(user_info.counts.begin(), user_info.counts.end(),
                      standard_info.counts.begin(), standard_info.counts.end());
    if (result == std::pair(user_info.counts.end(), standard_info.counts.end()))
    {
        quitf(TResult::_ok,
              "user solution is the same quality of standard solution");
    }
    if (*(result.first) > *(result.second))
    {
        quitf(TResult::_ok,
              "Warning! user solution is better than standard solution");
    }
    // assert(*(result.first) < *(result.second));
    const auto mismatched_index = result.first - user_info.counts.begin();
    if (mismatched_index >= 2)
    {
        (void)fprintf(stdout, "%f\n", subtask_2_score); // score for CMS
        // testlib style partial, score is ranged [0, 200]
        quitf(_pc(static_cast<int>(200 * subtask_2_score)),
              "mismatch at the %dth largest step count, "
              "user count: %d, "
              "standard count: %d.",
              static_cast<int>(mismatched_index + 1), *(result.first),
              *(result.second));
    }
    if (mismatched_index >= 1)
    {
        (void)fprintf(stdout, "%f\n", subtask_1_score); // score for CMS
        // testlib style partial, score is ranged [0, 200]
        quitf(_pc(static_cast<int>(200 * subtask_1_score)),
              "mismatch at the %dth largest step count, "
              "user count: %d, "
              "standard count: %d.",
              static_cast<int>(mismatched_index + 1), *(result.first),
              *(result.second));
    }
    quitf(TResult::_wa,
          "mismatch at the %dth largest step count, "
          "user count: %d, "
          "standard count: %d.",
          static_cast<int>(mismatched_index + 1), *(result.first),
          *(result.second));
}
