#include "testlib.h"
#include <charconv>
#include <cstdint>
#include <set>
#include <string>
#include <string_view>
using i64 = std::int64_t;
constexpr int SUBTASKS = 1;
constexpr int MX_N = 300'000;
constexpr int MX_K = 300'000;
constexpr int MX_E = 300'000;
constexpr i64 MX_COMPLEXITY_BOUND = 300'000;

int subtask(const std::vector<std::string_view> &args)
{
    auto itr = std::find(args.begin(), args.end(), "--testcase");
    ensuref(itr < args.end(), "--testcase is required.");
    ensuref(itr + 1 < args.end(),
            "--testcase must be followed by an argument.");
    const auto &str_v = *(itr + 1);
    int subtask{-1};
    auto result = std::from_chars(str_v.begin(), str_v.end(), subtask);
    ensuref(result.ec == std::errc{}, "Fail to parse: %.*s",
            static_cast<int>(str_v.length()), str_v.data());
    return subtask;
}

int main(int argc, char *argv[])
{
    registerValidation(argc, argv);
    const int sub_task_num = subtask({argv + 1, argv + argc});
    ensuref(0 <= sub_task_num && sub_task_num <= SUBTASKS, "subtask = %d",
            sub_task_num);
    const int input_n = inf.readInt(0, MX_N, "n");
    inf.readChar(' ');
    const int input_k = inf.readInt(2, MX_K, "k");
    inf.readChar(' ');
    const int input_e = inf.readInt(2, MX_E, "e");
    inf.readChar('\n');
    ensuref(input_n <= input_e - 1, "n <= e - 1 => %d <= %d", input_n,
            input_e - 1);
    ensuref(input_k <= input_e, "k <= e => %d <= %d", input_k, input_e);
    const auto bound_limit = static_cast<i64>(input_k) * (input_e - input_n);
    // i64 should use some weird printf format but I don't like it.
    ensuref(1 <= bound_limit && bound_limit <= MX_COMPLEXITY_BOUND,
            "1 <= bound_limit <= MX_COMPLEXITY_BOUND => 1 <= %s <= %s",
            std::to_string(bound_limit).c_str(),
            std::to_string(MX_COMPLEXITY_BOUND).c_str());
    {
        std::set<int> a_set{};
        for (int i = 0; i < input_n; ++i)
        {
            const auto var_name = "a_" + std::to_string(i);
            const int a_i = inf.readInt(1, input_e - 1, var_name);
            if (i != input_n - 1)
            {
                inf.readChar(' ');
            }
            auto itr = a_set.insert(a_i);
            ensuref(itr.second, "%.*s is %d and duplicates.",
                    static_cast<int>(var_name.size()), var_name.data(), a_i);
        }
        inf.readChar('\n');
    }
    {
        std::set<int> s_set{};
        for (int j = 0; j < input_k; ++j)
        {
            const auto var_name = "s_" + std::to_string(j);
            const int s_j = inf.readInt(1, input_e, var_name);
            if (j != input_k - 1)
            {
                inf.readChar(' ');
            }
            auto itr = s_set.insert(s_j);
            ensuref(itr.second, "%.*s is %d and duplicates.",
                    static_cast<int>(var_name.size()), var_name.data(), s_j);
        }
        inf.readChar('\n');
    }
    inf.readEof();
}
