#include "search.h"

#include <chrono>
#include <limits>

#include "chess/all.h"
#include "utils/timer.h"
#include "evaluate.h"

namespace sonic {

// Negamax search with alpha-beta pruning.
int negamax(Position& pos, SearchInfo& search_info, int alpha, int beta, int depth, Move& best_move) {
    search_info.nodes++;
    if(depth == 0 || search_info.time_out()) {
        return evaluate(pos);
    }
    MoveList movelist;
    generate_moves<GenType::ALL>(pos, movelist);
    best_move = Move::null_move();
    for(Move m : movelist) {
        Position new_pos(pos);
        if(!new_pos.make_move(m)) {
            continue;
        }
        Move tmp;
        int score = -negamax(new_pos, search_info, -beta, -alpha, depth - 1, tmp);
        if(score > alpha) {
            alpha = score;
            best_move = m;
            if(alpha >= beta) {
                break;
            }
        }
    }
    return alpha;
}

void search(Position& pos, SearchInfo& search_info) {
    // Iterative deepening
    Move best_move = Move::null_move();
    for(int depth = 1; depth <= search_info.max_depth; depth++) {
        int score = negamax(pos, search_info, std::numeric_limits<int>::min() / 2, std::numeric_limits<int>::max() / 2, depth, best_move);
        std::uint64_t ms = time_elapsed(search_info.start_time);
        std::cout << "info depth " << depth << " seldepth " << depth;
        std::cout << " score cp " << score << " nodes " << search_info.nodes;
        std::cout << " nps " << (search_info.nodes * 1000) / (ms + 1);
        std::cout << " time " << ms << std::endl;
        if(search_info.time_out()) {
            break;
        }
    }
    std::cout << "bestmove " << best_move.to_string() << std::endl;
}

} // namespace sonic