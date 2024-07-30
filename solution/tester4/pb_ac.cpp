#include <unordered_set>
#include <iostream>
#include <cstdint>
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
    std::unordered_set<std::uint64_t> seen;
    do {
        seen.insert(s);
        s = next(s, d);
    } while(seen.find(s) == seen.end());
    return s;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    unsigned n, d;
    std::cin >> n >> d;
    while (n--) {
        std::uint64_t s;
        std::cin >> s;
        std::cout << solve(s, d) << '\n';
    }
}
