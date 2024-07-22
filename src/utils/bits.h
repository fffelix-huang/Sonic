#pragma once

#include <cstdint>

namespace sonic {

constexpr int popcount(std::uint64_t num) {
#if defined(NO_POPCNT)
    std::uint64_t x = board_;
    x -= (x >> 1) & 0x5555555555555555;
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F;
    return (x * 0x0101010101010101) >> 56;
#elif defined(_MSC_VER) && defined(_WIN64)
    return _mm_popcnt_u64(num);
#elif defined(_MSC_VER)
    return __popcnt(num) + __popcnt(num >> 32);
#else
    return __builtin_popcountll(num);
#endif
}

constexpr int lsb(std::uint64_t num) {
    return __builtin_ctzll(num);
}

} // namespace sonic
