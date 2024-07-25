#pragma once

#include <cstdint>
#include <vector>

#include "../chess/all.h"

namespace sonic {

struct PerftTest {
    std::string fen;
    std::uint64_t expected_node_count;
    int depth;
};

std::uint64_t perft(Position& pos, int depth);
void bench_perft();

} // namespace sonic