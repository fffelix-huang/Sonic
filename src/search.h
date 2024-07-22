#pragma once

#include <chrono>
#include <cstdint>
#include <limits>

#include "chess/all.h"

namespace sonic {

struct SearchInfo {
    // Total nodes searched.
    std::uint64_t nodes = 0;
    std::uint64_t max_nodes = std::numeric_limits<std::uint64_t>::max() / 2;

    // Start time of the search.
    std::chrono::steady_clock::time_point start_time;
    std::uint32_t max_time = std::numeric_limits<std::uint32_t>::max() / 2;

    // Current search depth.
    std::uint8_t depth = 0;
    std::uint8_t max_depth = 150;

    Move best_move = Move::null_move();

    bool stop = false;
};

} // namespace sonic