#include <algorithm>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>

inline std::uint64_t reordered_min(const std::array<unsigned, 10> &counter) {
    std::uint64_t r{0};
    for (std::size_t i{0}; i < 10; ++i) {
        unsigned c{counter[i]};
        while (c--) {
            r = 10 * r + i;
        }
    }
    return r;
}

inline std::uint64_t reordered_max(const std::array<unsigned, 10> &counter) {
    std::uint64_t r{0};
    for (std::size_t i{9}; i < 10; --i) {
        unsigned c{counter[i]};
        while (c--) {
            r = 10 * r + i;
        }
    }
    return r;
}

inline std::uint64_t next(std::uint64_t s, unsigned d) {
    std::array<unsigned, 10> counter{};
    while (d--) {
        ++counter[s % 10];
        s /= 10;
    }
    const std::uint64_t min_s{reordered_min(counter)};
    const std::uint64_t max_s{reordered_max(counter)};
    return max_s - min_s;
}

inline std::uint64_t solve(std::uint64_t s, unsigned d) {
    // Since the longest step for a single number s does not exceed 30,
    // it is faster to simply use an array of continuous space to check
    // whether an element exists in O(n) time.
    std::vector<std::uint64_t> seen;
    do {
        seen.push_back(s);
        s = next(s, d);
    } while(std::find(seen.begin(), seen.end(), s) == seen.end());
    return s;
}

int main() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
    
    unsigned n, d;
    std::cin >> n >> d;
    while (n--) {
        std::uint64_t s;
        std::cin >> s;
        std::cout << solve(s, d) << '\n';
    }
}
