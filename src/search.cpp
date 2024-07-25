#include "search.h"

#include <chrono>
#include <limits>

#include "chess/all.h"
#include "utils/timer.h"
#include "evaluate.h"
#include "movesort.h"

namespace sonic {

int qsearch(Position& pos, SearchInfo& search_info, int alpha, int beta) {
    search_info.nodes++;
    if(pos.rule50_ply() >= 100) {
        return 0;
    }
    int score = evaluate(pos);
    if(search_info.time_out()) {
        return score;
    }
    if(pos.game_ply() > MAX_DEPTH - 1) {
        return score;
    }
    if(score >= beta) {
        return score;
    }
    alpha = std::max(alpha, score);
    MoveList captures;
    generate_moves<GenType::CAPTURE>(pos, captures);
    sort_moves(pos, captures);
    for(Move m : captures) {
        UndoInfo info;
        if(!pos.make_move(m, info)) {
            pos.unmake_move(info);
            continue;
        }
        int score = -qsearch(pos, search_info, -beta, -alpha);
        pos.unmake_move(info);
        if(score > alpha) {
            alpha = score;
            if(alpha >= beta) {
                break;
            }
        }
    }
    return alpha;
}

// Negamax search with alpha-beta pruning.
Value negamax(Position& pos, SearchInfo& search_info, Value alpha, Value beta, int depth, Move& move) {
    if(pos.rule50_ply() >= 100) {
        return VALUE_DRAW;
    }
    if(search_info.time_out()) {
        return evaluate(pos);
    }
    if(depth <= 0) {
        return qsearch(pos, search_info, alpha, beta);
    }
    search_info.nodes++;
    // Perform search on the best move from previous search first.
    if(move != Move::null_move()) {
        UndoInfo info;
        assert(pos.make_move(move, info));
        Move tmp = Move::null_move();
        Value score = -negamax(pos, search_info, -beta, -alpha, depth - 1, tmp);
        pos.unmake_move(info);
        if(score > alpha) {
            alpha = score;
            if(alpha >= beta) {
                return alpha;
            }
        }
    }
    MoveList movelist;
    generate_moves<GenType::ALL>(pos, movelist);
    sort_moves(pos, movelist);
    Move best_move = move;
    int legal_moves = 0;
    for(Move m : movelist) {
        if(m == move) {
            legal_moves++;
            continue;
        }
        UndoInfo info;
        if(!pos.make_move(m, info)) {
            pos.unmake_move(info);
            continue;
        }
        legal_moves++;
        Move tmp = Move::null_move();
        Value score = -negamax(pos, search_info, -beta, -alpha, depth - 1, tmp);
        pos.unmake_move(info);
        if(score > alpha) {
            alpha = score;
            best_move = m;
            if(alpha >= beta) {
                break;
            }
        }
    }
    if(legal_moves == 0) {
        // Checkmate or Stalemate.
        return pos.in_check() ? mated_in(search_info.depth) : VALUE_DRAW;
    }
    move = best_move;
    return alpha;
}

void search(Position& pos, SearchInfo& search_info) {
    // Iterative deepening
    Move best_move = Move::null_move();
    for(int depth = 1; depth <= search_info.max_depth; depth++) {
        int score = negamax(pos, search_info, -VALUE_INF, VALUE_INF, depth, best_move);
        std::uint64_t ms = time_elapsed(search_info.start_time);
        std::cout << "info depth " << depth << " seldepth " << depth;
        std::cout << " score cp " << score << " nodes " << search_info.nodes;
        std::cout << " nps " << (search_info.nodes * 1000) / (ms + 1);
        std::cout << " time " << ms;
        std::cout << " pv " << best_move.to_string() << std::endl;
        if(search_info.time_out()) {
            break;
        }
    }
    std::cout << "bestmove " << best_move.to_string() << std::endl;
}

} // namespace sonic