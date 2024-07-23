#include "benchmark.h"

#include <vector>
#include <string>

#include "../chess/all.h"
#include "../utils/strings.h"
#include "../utils/timer.h"
#include "../uci.h"
#include "../search.h"

namespace {

std::vector<std::string> bench_positions = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 10",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 11",
    "4rrk1/pp1n3p/3q2pQ/2p1pb2/2PP4/2P3N1/P2B2PP/4RRK1 b - - 7 19",
    "rq3rk1/ppp2ppp/1bnpb3/3N2B1/3NP3/7P/PPPQ1PP1/2KR3R w - - 7 14 moves d4e6",
    "r1bq1r1k/1pp1n1pp/1p1p4/4p2Q/4Pp2/1BNP4/PPP2PPP/3R1RK1 w - - 2 14 moves g2g4",
    "r3r1k1/2p2ppp/p1p1bn2/8/1q2P3/2NPQN2/PPP3PP/R4RK1 b - - 2 15",
    "r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
    "r1bq1rk1/ppp1nppp/4n3/3p3Q/3P4/1BP1B3/PP1N2PP/R4RK1 w - - 1 16",
    "4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17",
};

} // namespace

namespace sonic {

void run_bench() {
    const std::vector<std::string> go_params = {"go", "depth", "7"};
    std::uint64_t node_count = 0;
    TimePoint start = current_time();
    Position pos;
    for(int i = 0; i < bench_positions.size(); i++) {
        std::string fen = "position fen " + bench_positions[i];
        std::vector<std::string> params = split_string(fen, ' ');
        SearchInfo search_info;
        parse_position(pos, search_info, params);
        parse_go(pos, search_info, go_params);
        std::cout << "Position [" << i + 1 << "/" << bench_positions.size() << "]" << " (" << pos.fen() << ")" << std::endl;
        search(pos, search_info);
        node_count += search_info.nodes;
        std::cout << "\n";
    }
    std::uint64_t ms = time_elapsed(start);
    std::cout << std::string(20, '=') << std::endl;
    std::cout << "Total time (ms) : " << ms << std::endl;
    std::cout << "Nodes searched  : " << node_count << std::endl;
    std::cout << "Nodes/second    : " << (node_count * 1000) / (ms + 1) << std::endl;
}

} // namespace sonic