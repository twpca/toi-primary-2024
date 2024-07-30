#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using i08 = std::int8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

constexpr i32 MX_E = 300'000;
const size_t SQRT_MX_E = static_cast<size_t>(std::sqrt(MX_E));
constexpr i64 MX_COMPLEXITY_BOUND = 300'000;

class arg_error : std::runtime_error
{
public:
    arg_error()
        : std::runtime_error("arg_error: type subtask_index serial_number seed "
                             "min_e max_e "
                             "min_n max_n "
                             "min_k max_k")
    {
    }
};

struct final_data_t
{
    size_t e;
    std::vector<size_t> steps;
    std::vector<size_t> hole_indices;
};

template <class T> struct my_span_t
{
    T *st;
    T *ed;
    T pop()
    {
        if (st >= ed)
        {
            throw arg_error();
        }
        return *(st++);
    }
    void assert_empty() const
    {
        if (st < ed)
        {
            throw arg_error();
        }
    }
};

struct generator_random_args_t
{
    size_t min_n;
    size_t max_n;
    size_t min_k;
    size_t max_k;
    size_t min_e;
    size_t max_e;
    explicit generator_random_args_t(my_span_t<char *> &args)
    {
        min_e = std::stoul(args.pop());
        max_e = std::stoul(args.pop());
        min_n = std::stoul(args.pop());
        max_n = std::stoul(args.pop());
        min_k = std::stoul(args.pop());
        max_k = std::stoul(args.pop());
    }
    void assert_valid() const
    {
        if (min_k < 2)
        {
            throw std::runtime_error("min_k < 2. Wrong.");
        }
        if (min_k > max_k)
        {
            throw std::runtime_error("min_k > max_k. Wrong.");
        }
        if (min_e < min_n + 1)
        {
            throw std::runtime_error("min_e < min_n + 1. Wrong.");
        }
        if (min_e < min_k + 1)
        {
            throw std::runtime_error("min_e < min_k + 1. Wrong.");
        }
        if (min_e > max_e)
        {
            throw std::runtime_error("min_e > max_e. Wrong.");
        }
    }
};

template <class T> struct my_dist
{
    static constexpr auto max_v =
        std::numeric_limits<std::mt19937::result_type>::max();
    T st;
    T size;
    std::mt19937::result_type bad_min;
    /* Note that this doesn't work if end - start + 1 > max value of T */
    my_dist(T start, T end)
        : st(start), size(end - start + 1), bad_min(max_v / size * size)
    {
    }
    T operator()(std::mt19937 &rng) const
    {
        while (true)
        {
            auto new_value = rng();
            if (new_value < bad_min)
            {
                return st + new_value % size;
            }
        }
    }
};

template <class T> using uids_t = my_dist<T>;

auto sample_n(size_t n, size_t start, size_t end, std::mt19937 &rng)
{
    const auto size = end - start + 1;
    if (n > size)
    {
        throw std::runtime_error(
            "Call of sample_n doesn't satisfy precondition.");
    }
    std::vector<size_t> arr(size);
    std::iota(arr.begin(), arr.end(), start);
    for (size_t i = 0; i < n; ++i)
    {
        auto random_index = uids_t<size_t>(i, arr.size() - 1)(rng);
        std::swap(arr.at(i), arr.at(random_index));
    }
    arr.resize(n);
    return arr;
}

/*
std library can change their implementation of shuffle at any version.

Use a fixed version.
*/
template <class T> void my_shuffle(T start, T end, std::mt19937 &rng)
{
    static_assert(
        std::is_same<
            std::random_access_iterator_tag,
            typename std::iterator_traits<T>::iterator_category>::value,
        "T is not random_access_iterator.\n");
    const auto size = end - start;
    for (size_t i = 0; i < size; ++i)
    {
        auto random_index = uids_t<size_t>(i, size - 1)(rng);
        std::swap(*(start + i), *(start + random_index));
    }
}

template <class T>
auto output(size_t out_e, const T &out_a, const T &out_s, std::ostream &ous)
{
    /*
    n k e
    a_1, ..., a_n
    s_1, ..., s_k
    */
    ous << out_a.size() << " " << out_s.size() << " " << out_e << '\n';
    for (auto &val : out_a)
    {
        ous << val << (&val == &out_a.back() ? '\n' : ' ');
    }
    for (auto &val : out_s)
    {
        ous << val << (&val == &out_s.back() ? '\n' : ' ');
    }
    ous.flush();
}

void GenerateRandom(const generator_random_args_t &args, std::mt19937 &rng,
                    std::ostream &ous)
{
    const size_t res_e = uids_t<size_t>(args.min_e, args.max_e)(rng);
    const size_t res_n =
        uids_t<size_t>(args.min_n, std::min(args.max_n, res_e - 1))(rng);
    const size_t res_k =
        uids_t<size_t>(args.min_k, std::min(args.max_k, res_e - 1))(rng);
    const auto hole_indices = sample_n(res_n, 1, res_e - 1, rng);
    /* Prevent generating step with e length. */
    const auto steps = sample_n(res_k, 1, res_e - 1, rng);

    output(res_e, hole_indices, steps, ous);
}

struct generator_path_args_t
{
    size_t min_k;
    size_t max_k;
    size_t min_branch_factor;
    size_t max_branch_factor;
    explicit generator_path_args_t(my_span_t<char *> &args)
    {
        min_k = std::stoul(args.pop());
        max_k = std::stoul(args.pop());
        min_branch_factor = std::stoul(args.pop());
        max_branch_factor = std::stoul(args.pop());
    }
    void assert_valid() const
    {
        if (min_k < 2)
        {
            throw std::runtime_error("min_k < 2. Wrong.");
        }
        if (min_k > max_k)
        {
            throw std::runtime_error("min_k > max_k. Wrong.");
        }
        if (min_branch_factor > max_branch_factor)
        {
            throw std::runtime_error("min_k > max_k. Wrong.");
        }
    }
};

auto GeneratePath(const generator_path_args_t &args, std::mt19937 &rng,
                  std::ostream* ous) -> final_data_t
{
    const size_t number_of_steps = uids_t<size_t>(args.min_k, args.max_k)(rng);
    const size_t number_of_useful_k = std::min(SQRT_MX_E, number_of_steps / 2);
    auto useful_s =
        sample_n(number_of_useful_k, 3,
                 2 + std::max(SQRT_MX_E, number_of_steps / 2) * 2, rng);
    std::sort(useful_s.begin(), useful_s.end());

    /* ssc is the number of safe squares.*/
    size_t ssc = 0;
    const size_t max_ssc = MX_COMPLEXITY_BOUND / number_of_steps + 1;
    const size_t MX_E_1 = MX_E + 1;
    std::vector<i08> holes(MX_E_1, 1);
    holes[0] = 0;
    constexpr size_t unset = 0xFFFF'FFFF;
    size_t last = unset;
    auto branch_factor_dist =
        uids_t<size_t>(args.min_branch_factor, args.max_branch_factor);
    std::vector<size_t> hole_indices{};
    for (size_t pos = 0; pos < MX_E_1; ++pos)
    {
        if (holes[pos] == 1)
        {
            hole_indices.push_back(pos);
            continue;
        }
        last = pos;
        ++ssc;
        if (ssc >= max_ssc)
        {
            break;
        }
        const auto branch_factor = branch_factor_dist(rng);
        const auto sample_indices =
            sample_n(std::min(branch_factor, number_of_useful_k), 0,
                     number_of_useful_k - 1, rng);
        for (auto index : sample_indices)
        {
            auto next_pos = pos + useful_s.at(index);
            if (next_pos < MX_E_1)
            {
                holes.at(next_pos) = 0;
            }
        }
    }
    if (last == unset)
    {
        throw std::runtime_error("Fail to create a test data. last == unset");
    }
    if (useful_s.at(useful_s.size() - 1) >= last)
    {
        throw std::runtime_error("Fail to create a test data. "
                                 "Too many safe squares. "
                                 "Try to lower the branch_factor.");
    }
    const auto min_useless_step = last - useful_s.at(0) + 1;
    std::cerr << "min_useless_step: " << min_useless_step << "\n";
    for (auto next_useless_step = min_useless_step;
         useful_s.size() < number_of_steps && next_useless_step < last;)
    {
        useful_s.push_back(next_useless_step++);
    }
    if (useful_s.size() != number_of_steps)
    {
        std::cerr << "Warning: Expect k==" << number_of_steps;
        std::cerr << " result: k==" << useful_s.size() << "\n";
    }
    my_shuffle(useful_s.begin(), useful_s.end(), rng);
    my_shuffle(hole_indices.begin(), hole_indices.end(), rng);
    if(ous != nullptr)
    {
        output(last, hole_indices, useful_s, *ous);
    }
    return {last, useful_s, hole_indices};
}

struct cost_t
{
    std::vector<std::pair<i32, i32>> step_to_counts;
    i32 last_step;
    /*
    time complexity: O(sqrt(e))
    */
    bool operator<(const cost_t &o) const
    {
        return step_to_counts < o.step_to_counts;
    }
    bool operator==(const cost_t &o) const
    {
        return step_to_counts == o.step_to_counts;
    }
};

auto solve(size_t E, const std::vector<i32> &steps, const std::vector<i08> &holes)
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
            auto new_cost = cost_t{
                {},
                step,
            };
            new_cost.step_to_counts.resize(cur_cost.step_to_counts.size() +
                                            1);
            auto end = std::copy(cur_cost.step_to_counts.begin(),
                                    cur_cost.step_to_counts.end(),
                                    new_cost.step_to_counts.begin());
            new_cost.step_to_counts.erase(end,
                                            new_cost.step_to_counts.end());
            auto itr = std::lower_bound(new_cost.step_to_counts.begin(),
                                        new_cost.step_to_counts.end(),
                                        std::pair<i32, i32>{step, E},
                                        std::greater());
            if (itr < new_cost.step_to_counts.end() && itr->first == step)
            {
                itr->second++;
            }
            else
            {
                new_cost.step_to_counts.insert(itr, {step, 1});
            }
            auto &target_cost = min_cost[cur_pos + step];
            if (target_cost < new_cost)
            {
                target_cost = std::move(new_cost);
            }
        }
        // Release memory
        cur_cost.step_to_counts = decltype(cur_cost.step_to_counts){};
    }
    std::map<i32, i32> ans{};
    if (!(min_cost.back() == inf))
    {
        size_t cur_pos = E - 1;
        for (; E > cur_pos && cur_pos > 0;)
        {
            auto &now = min_cost[cur_pos];
            ans[now.last_step]++;
            cur_pos -= now.last_step;
        }
    }
    return ans;
}

void GeneratePathAntiPartial1(const generator_path_args_t &args, std::mt19937 &rng,
                              std::ostream &ous)
{
    auto test_data = GeneratePath(args, rng, nullptr);
    std::sort(test_data.steps.begin(), test_data.steps.end());
    std::map<i32, i32> ans{};
    {
        std::vector<i32> steps(test_data.steps.begin(), test_data.steps.end());
        std::vector<i08> holes(test_data.e + 1);
        for(auto &hole_index: test_data.hole_indices)
        {
            holes.at(hole_index) = 1;
        }
        ans = solve(test_data.e + 1, steps, holes);
    }
    if(ans.empty())
    {
        throw std::runtime_error("GeneratePathAntiPartial1 no answer!?");
    }
    for(;!test_data.steps.empty() && test_data.steps.back() > ans.rbegin()->first;)
    {
        test_data.steps.pop_back();
    }
    const auto min_useless_step = test_data.e - test_data.steps.at(0) + 1;
    test_data.steps.push_back(min_useless_step);
    my_shuffle(test_data.steps.begin(), test_data.steps.end(), rng);
    output(test_data.e, test_data.hole_indices, test_data.steps, ous);
}

int RunThrowableMain(int argc, char **argv)
{
    auto args = my_span_t<char *>{argv + 1, argv + argc};
    const std::string_view type = args.pop();
    const size_t subtask_index = std::stoul(args.pop());
    const size_t serial_number = std::stoul(args.pop());
    const size_t seed = std::stoul(args.pop(), nullptr, 0);
    constexpr int subtask_num = 2;
    if (subtask_index >= subtask_num || subtask_index <= 0)
    {
        std::ostringstream oss;
        oss << "subtask_index = " << subtask_index << " must in [";
        for (int i = 1; i <= subtask_num; ++i)
        {
            oss << i << ",";
        }
        oss << "]";
        throw std::runtime_error(oss.str());
    }
    std::string file_name;
    {
        constexpr int hex_width = 8;
        std::ostringstream oss;
        oss << "./" << subtask_index;
        oss << "_" << std::setfill('0') << std::setw(4) << serial_number;
        oss << "_" << type;
        oss << "_" << std::setfill('0') << std::hex << std::setw(hex_width)
            << seed;
        oss << ".in";
        file_name = oss.str();
    }
    std::mt19937 rng(seed);
    if (type == "random")
    {
        auto generator_args = generator_random_args_t(args);
        generator_args.assert_valid();
        args.assert_empty();
        std::ofstream ofp(file_name);
        GenerateRandom(generator_args, rng, ofp);
    }
    else if (type == "path")
    {
        auto generator_args = generator_path_args_t(args);
        generator_args.assert_valid();
        args.assert_empty();
        std::ofstream ofp(file_name);
        GeneratePath(generator_args, rng, &ofp);
    }
    else if (type == "path2")
    {
        auto generator_args = generator_path_args_t(args);
        generator_args.assert_valid();
        args.assert_empty();
        std::ofstream ofp(file_name);
        GeneratePathAntiPartial1(generator_args, rng, ofp);
    }
    else
    {
        std::ostringstream oss;
        oss << "unknown type: " << type;
        throw std::runtime_error(oss.str());
    }
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return RunThrowableMain(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << "RunThrowableMain: " << err.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception catch\n";
        return 2;
    }
}
