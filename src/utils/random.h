#pragma once

#include <chrono>
#include <cstdint>

namespace sonic {

inline std::uint64_t rng() {
    static std::uint64_t SEED = std::chrono::steady_clock::now().time_since_epoch().count();
    SEED ^= SEED << 7;
    SEED ^= SEED >> 9;
    return SEED;
}

}  // namespace sonic
