#include <unordered_set>
#include <iostream>
#include <array>

inline int reordered_min(const std::array<int, 10> &counter) {
    int r{0};
    for (int i{0}; i < 10; ++i) {
        int c{counter[i]};
        while (c--) {
            r = 10 * r + i;
        }
    }
    return r;
}

inline int reordered_max(const std::array<int, 10> &counter) {
    int r{0};
    for (int i{9}; i >= 0; --i) {
        int c{counter[i]};
        while (c--) {
            r = 10 * r + i;
        }
    }
    return r;
}

inline int next(int s, int d) {
    std::array<int, 10> counter{};
    while (d--) {
        ++counter[s % 10];
        s /= 10;
    }
    const int min_s{reordered_min(counter)};
    const int max_s{reordered_max(counter)};
    return max_s - min_s;
}

inline int solve(int s, int d) {
    std::unordered_set<int> seen;
    do {
        seen.insert(s);
        s = next(s, d);
    } while(seen.find(s) == seen.end());
    return s;
}

int main() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
    
    int n, d;
    std::cin >> n >> d;
    while (n--) {
        int s;
        std::cin >> s;
        std::cout << solve(s, d) << '\n';
    }
}
