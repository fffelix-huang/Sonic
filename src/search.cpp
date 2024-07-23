#include "search.h"

#include <chrono>
#include <limits>

#include "chess/all.h"
#include "utils/timer.h"
#include "evaluate.h"

namespace sonic {

int negamax(Position& pos, SearchInfo& search_info, int depth, Move& best_move) {
    search_info.nodes++;
    if(depth == 0) {
        return evaluate(pos);
    }
    MoveList movelist;
    generate_moves<GenType::ALL>(pos, movelist);
    int best_score = std::numeric_limits<int>::min();
    best_move = Move::null_move();
    for(Move m : movelist) {
        Position new_pos(pos);
        if(!new_pos.make_move(m)) {
            continue;
        }
        Move tmp;
        int score = -negamax(new_pos, search_info, depth - 1, tmp);
        if(score > best_score) {
            best_score = score;
            best_move = m;
        }
    }
    return best_score;
}

void search(Position& pos, SearchInfo& search_info) {
    // Iterative deepening
    Move best_move = Move::null_move();
    for(int depth = 1; depth <= search_info.max_depth; depth++) {
        int score = negamax(pos, search_info, depth, best_move);
        std::uint64_t ms = time_elapsed(search_info.start_time);
        std::cout << "info depth " << depth << " seldepth " << depth;
        std::cout << " score cp " << score << " nodes " << search_info.nodes;
        std::cout << " nps " << (search_info.nodes * 1000) / (ms + 1);
        std::cout << " time " << ms << std::endl;
    }
    std::cout << "bestmove " << best_move.to_string() << std::endl;
}

} // namespace sonic