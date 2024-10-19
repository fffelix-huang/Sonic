#pragma once

#include <chrono>
#include <cstdint>

namespace sonic {

using TimePoint = std::chrono::high_resolution_clock::time_point;

inline TimePoint current_time() { return std::chrono::high_resolution_clock::now(); }

inline std::uint64_t time_elapsed(TimePoint start) {
    TimePoint now = current_time();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

}  // namespace sonic