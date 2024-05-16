#include "testlib.h"
#include <unordered_set>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <array>
#include <iostream>

using u64  = std::uint64_t;
using uint = unsigned;
using sz   = std::size_t;

constexpr u64 pow10(u64 r) {
    u64 ret{1};
    while (r--) {
        ret *= 10;
    }
    return ret;
}

static constexpr uint SUBTASKS = 3;
static constexpr u64 MAX_TTS = 100000;
static constexpr int MIN_N[SUBTASKS + 1] = {1, 1, 1, 1};
static constexpr int MAX_N[SUBTASKS + 1] = {10000, 10000, 100, 10000};
static constexpr int MIN_D[SUBTASKS + 1] = {1, 3, 2, 2};
static constexpr int MAX_D[SUBTASKS + 1] = {10, 4, 6, 10};
// static constexpr u64 MIN_S[SUBTASKS + 1] = {1, 1, 1, 1};
// static constexpr u64 MAX_S[SUBTASKS] = {pow10(MIN_D[0]), pow10(MIN_D[1]), pow10(MIN_D[2])};

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

uint subtask(int argc, char* argv[]) {
    using namespace std::string_literals;
    for (int i{1}; i < argc; ++i) {
        if (argv[i-1] == "--testcase"s) {
            uint s{static_cast<uint>(std::stoi(argv[i]))};
            assert(s <= SUBTASKS);
            return s;
        }
    }
    assert(0);
    return -1;
}

int main(int argc, char* argv[]) {
    
    registerValidation(argc, argv);
    const uint s{subtask(argc, argv)};
    
    const uint n{static_cast<uint>(inf.readInt(MIN_N[s], MAX_N[s], "n"))};
    inf.readSpace();
    const uint d{static_cast<uint>(inf.readInt(MIN_D[s], MAX_D[s], "d"))};
    inf.readChar('\n');
	const u64 MAX_S{pow10(d)};
    
    u64 total_steps{0};
    for (uint i{1}; i <= n; ++i) {
        const u64 s{inf.readUnsignedLong(1, MAX_S - 1, "s_" + std::to_string(i))};
        ensuref(check_digits(s, d), "all digits are same");
        inf.readChar('\n');
        total_steps += steps_length(s, d);
    }
    
    ensuref(total_steps <= MAX_TTS, "total number of steps exceeded");
    
    inf.readEof();
}
