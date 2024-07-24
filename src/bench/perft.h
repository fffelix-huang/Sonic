#pragma once

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <vector>

#include "../chess/all.h"
#include "../utils/timer.h"

namespace sonic {

// Count number of leaf nodes.
std::uint64_t perft(Position& pos, int depth) {
    if(depth == 0) {
        return 1;
    }
    std::uint64_t node_count = 0;
    MoveList movelist;
    generate_moves<GenType::ALL>(pos, movelist);
    for(const Move& m : movelist) {
        UndoInfo info;
        if(!pos.make_move(m, info)) {
            pos.unmake_move(info);
            continue;
        }
        std::uint64_t count = perft(pos, depth - 1);
        node_count += count;
        pos.unmake_move(info);
    }
    return node_count;
}

struct PerftTest {
    std::string fen;
    std::uint64_t expected_node_count;
    int depth;
};

void bench_perft() {
    const std::vector<PerftTest> perft_tests = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 119060324ULL, 6},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 193690690ULL, 5},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 178633661ULL, 7},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 706045033ULL, 6},
        {"r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 706045033ULL, 6}, // Mirror
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 89941194ULL, 5},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 164075551ULL, 5},
    };

    for(std::size_t i = 0; i < perft_tests.size(); i++) {
        const auto& test = perft_tests[i];
        Position pos(test.fen);

        TimePoint start = current_time();
        std::uint64_t node_count = perft(pos, test.depth);
        std::uint64_t ms = time_elapsed(start);

        std::cout << "Position [" << i + 1 << "/" << perft_tests.size() << "]:";
        std::cout << std::left;
        std::cout << " depth " << std::setw(2) << test.depth;
        std::cout << " time " << std::setw(5) << ms;
        std::cout << " nodes " << std::setw(12) << node_count;
        std::cout << " nps " << std::setw(9) << (node_count * 1000) / (ms + 1);
        std::cout << " fen " << std::setw(87) << test.fen << std::endl;
        assert(node_count == test.expected_node_count);
    }
}

} // namespace sonic