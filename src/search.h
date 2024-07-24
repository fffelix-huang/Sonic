#pragma once

#include <chrono>
#include <cstdint>
#include <limits>
#include <thread>

#include "chess/all.h"
#include "utils/timer.h"

namespace sonic {

struct SearchInfo {
    // Total nodes searched.
    std::uint64_t nodes = 0;
    std::uint64_t max_nodes = std::numeric_limits<std::uint64_t>::max() / 2;

    // Start time of the search.
    TimePoint start_time;
    std::uint64_t max_time = std::numeric_limits<std::uint64_t>::max() / 2;

    // Current search depth.
    std::uint8_t depth = 0;
    std::uint8_t max_depth = 150;

    bool stop = false;

    bool time_out() const { return stop || (max_time < time_elapsed(start_time)); }
};

void search(const Position& pos, SearchInfo& search_info);

} // namespace sonic