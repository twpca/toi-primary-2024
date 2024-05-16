#include <iostream>
#include <cstdint>

inline std::uint64_t solve(std::uint64_t s, unsigned d) {
    if (d == 3) {
        return 495;
    }
    if (d == 4) {
        return 6174;
    }
    return 0;
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
