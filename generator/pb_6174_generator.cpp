#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <vector>
#include <array>

using u64  = std::uint64_t;
using uint = unsigned;
using sz   = std::size_t;
template<typename T> using unif  = std::uniform_int_distribution<T>;
template<typename T> using unifr = std::uniform_real_distribution<T>;

static constexpr uint SUBTASKS = 3;

static const std::vector<std::vector<u64>> bcycle{
    {},
    {},
    {9, 81, 63, 27, 45},
    {495},
    {6174},
    {71973, 83952, 74943, 62964,
     82962, 75933, 63954, 61974,
     53955, 59994},
    {631764,
     549945,
     420876, 851742, 750843, 840852, 860832, 862632, 642654},
    {7509843, 9529641, 8719722, 8649432, 7519743, 8429652, 7619733, 8439552},
    {63317664,
     97508421,
     86526432, 64308654, 83208762,
     86308632, 86326632, 64326654, 43208466, 85317642, 75308643, 84308652},
    {554999445,
     864197532,
     883098612, 976494321, 874197522, 865296432, 763197633, 844296552, 762098733, 964395531, 863098632, 965296431, 873197622, 865395432, 753098643, 954197541},
    {6333176664,
     9753086421,
     9975084201,
     8653266432, 6433086654, 8332087662,
     6431088654, 8732087622, 8655264432,
     6543086544, 8321088762, 8765264322,
     9775084221, 9755084421, 9751088421,
     8633086632, 8633266632, 6433266654, 4332084666, 8533176642, 7533086643, 8433086652
    }
};

inline constexpr u64 pow10(u64 r) {
    u64 ret{1};
    while (r--) {
        ret *= 10;
    }
    return ret;
}

inline u64 reordered_min(const std::array<uint, 10> &counter) {
    u64 r{0};
    for (sz i{0}; i < 10; ++i) {
        uint c{counter[i]};
        while (c--) {
            r = 10 * r + i;
        }
    }
    return r;
}

inline u64 reordered_max(const std::array<uint, 10> &counter) {
    u64 r{0};
    for (sz i{9}; i < 10; --i) {
        uint c{counter[i]};
        while (c--) {
            r = 10 * r + i;
        }
    }
    return r;
}

inline u64 next(u64 s, uint d) {
    std::array<uint, 10> counter{};
    while (d--) {
        ++counter[s % 10];
        s /= 10;
    }
    const u64 min_s{reordered_min(counter)};
    const u64 max_s{reordered_max(counter)};
    return max_s - min_s;
}

inline u64 steps_length(u64 s, uint d) {
    std::unordered_set<u64> seen;
    do {
        seen.insert(s);
        s = next(s, d);
    } while(seen.find(s) == seen.end());
    return seen.size();
}

inline bool check_digits(u64 s, uint d) {
    if (d == 0) {
        return false;
    }
    for (const u64 g{s % 10}; --d; ) {
        s /= 10;
        if (s % 10 != g) {
            return true;
        }
    };
    return false;
}

void gen_input(const std::string &fn, std::mt19937 &rng,
               const uint min_n, const uint max_n,
               const uint min_d, const uint max_d,
               const double prob_cyc = 0.0, const uint step_limit = 0) {
    
    const uint n{unif<uint>(min_n, max_n)(rng)};
    const uint d{unif<uint>(min_d, max_d)(rng)};
    const u64 min_s{1}, max_s{pow10(d) - 1};
    unif<u64>     unif_s(min_s, max_s);
    unifr<double> unif_r01(0.0, 1.0);
    std::vector<u64> cyc(bcycle[d]);
    std::shuffle(cyc.begin(), cyc.end(), rng);
    std::ofstream ofs(fn);
    
    ofs << n << " " << d << "\n";
    for (uint i{0}; i < n; ++i) {
        u64 s;
        if (!cyc.empty() && unif_r01(rng) < prob_cyc) {
            s = cyc.back();
            cyc.pop_back();
        }
        else {
            do {
                s = unif_s(rng);
            } while (!check_digits(s, d) || step_limit != 0 && steps_length(s, d) > step_limit);
        }
        ofs << s << "\n";
    }
}

void gen_sub1(std::mt19937 &rng) {
    const uint serial_sub1{4};
    for (uint s{0}; s < serial_sub1; ++s) {
        std::ostringstream fn_os;
        fn_os << "1_" << std::setfill('0') << std::setw(2) << s << ".in";
        switch (s) {
            case 0:
                gen_input(fn_os.str(), rng, 90, 100, 3, 3, 0.02);
                continue;
            case 1:
                gen_input(fn_os.str(), rng, 900, 1000, 4, 4, 0.002);
                continue;
            case 2:
                gen_input(fn_os.str(), rng, 1900, 2000, 3, 3, 0.001);
                continue;
            case 3:
                gen_input(fn_os.str(), rng, 9000, 10000, 4, 4, 0.0002);
                continue;
            default :
                assert(0);
        }
    }
}

void gen_sub2(std::mt19937 &rng) {
    const uint serial_sub2{10};
    for (uint s{0}; s < serial_sub2; ++s) {
        std::ostringstream fn_os;
        fn_os << "2_" << std::setfill('0') << std::setw(2) << s << ".in";
        const uint d{s / 2 + 2};
        assert(2 <= d && d <= 6);
        gen_input(fn_os.str(), rng, 90, 100, d, d, bcycle[d].size() / 100.0);
    }
}

void gen_sub3(std::mt19937 &rng) {
    const uint serial_sub3{20};
    gen_input("3_00.in", rng, 190, 200, 2, 2, bcycle[2].size() / 200.0);
    gen_input("3_01.in", rng, 1900, 2000, 3, 3, bcycle[3].size() / 2000.0);
    gen_input("3_02.in", rng, 9000, 10000, 4, 4, bcycle[4].size() / 10000.0);
    gen_input("3_03.in", rng, 9000, 10000, 5, 5, bcycle[5].size() / 10000.0);
    gen_input("3_04.in", rng, 9000, 10000, 5, 5, bcycle[5].size() / 10000.0);
    for (uint s{5}; s < serial_sub3; ++s) {
        std::ostringstream fn_os;
        fn_os << "3_" << std::setfill('0') << std::setw(2) << s << ".in";
        const uint d{(s - 5) / 3 + 6};
        assert(6 <= d && d <= 10);
		const uint c{s % 3};
		switch (c) {
			case 0 :
				gen_input(fn_os.str(), rng, 4000, 5000, d, d, 0.0);
				continue;
			case 1 :
				gen_input(fn_os.str(), rng, 4000, 5000, d, d, bcycle[d].size() / 5000.0);
				continue;
			case 2 :
				gen_input(fn_os.str(), rng, 9000, 10000, d, d, bcycle[d].size() / 10000.0, 11);
				continue;
			default :
				assert(0);
		}
    }
}

int main(int argc, const char *const argv[]) {
    if (argc != 3) {
        std::cerr << "./pb_6174_generator subtask seed\n";
        return 1;
    }

    const uint s{static_cast<uint>(std::stoi(argv[1]))};
    if (s <= 0 || SUBTASKS < s) {
        std::cerr << "subtask_index must in [1, " << SUBTASKS << "]\n";
        return 1;
    }
    
    const u64 seed{std::stoull(argv[2], nullptr, 0)};
    std::mt19937 rng(seed);
    
    switch (s) {
        case 1 :
            gen_sub1(rng);
            return 0;
        case 2 :
            gen_sub2(rng);
            return 0;
        case 3 :
            gen_sub3(rng);
            return 0;
        default :
            assert(0);
    }
}

