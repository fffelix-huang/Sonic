#include "search.h"

#include <algorithm>
#include <chrono>
#include <limits>

#include "chess/all.h"
#include "utils/timer.h"
#include "book.h"
#include "evaluate.h"
#include "movesort.h"
#include "tt.h"
#include "types.h"

namespace sonic {

Value qsearch(Position& pos, SearchInfo& search_info, int alpha, int beta) {
    int ply = search_info.depth;
    search_info.nodes++;
    search_info.seldepth = std::max(search_info.seldepth, ply);
    search_info.pv_length[ply] = 0;
    if(search_info.is_repetition(pos.hashkey()) || pos.rule50_ply() >= 100) {
        return VALUE_DRAW;
    }
    if(search_info.time_out()) {
        return VALUE_NONE;
    }
    Value score = evaluate(pos);
    if(ply > MAX_DEPTH - 1) {
        return score;
    }
    TTEntry entry = TT.get(pos);
    if(entry.key == pos.hashkey()) {
        if(entry.flag == TTFlag::TT_EXACT || (entry.flag == TTFlag::TT_ALPHA && entry.score <= alpha) || (entry.flag == TTFlag::TT_BETA && entry.score >= beta)) {
            return entry.score;
        }
    }
    if(score >= beta) {
        return score;
    }
    alpha = std::max(alpha, score);
    MoveList captures;
    generate_moves<GenType::CAPTURE>(pos, captures);
    sort_moves(pos, captures);
    Value best_score = -VALUE_INF;
    Move best_move = MOVE_NONE;
    TTFlag flag = TTFlag::TT_ALPHA;
    for(Move m : captures) {
        UndoInfo info;
        search_info.store_history(pos.hashkey());
        search_info.depth++;
        if(!pos.make_move(m, info)) {
            pos.unmake_move(info);
            search_info.depth--;
            continue;
        }
        Value score = -qsearch(pos, search_info, -beta, -alpha);
        pos.unmake_move(info);
        search_info.depth--;
        if(score > best_score) {
            best_score = score;
            best_move = m;
            if(score > alpha) {
                alpha = score;
                search_info.insert_pv(ply, m);
                flag = TTFlag::TT_EXACT;
                if(alpha >= beta) {
                    flag = TTFlag::TT_BETA;
                    break;
                }
            }
        }
    }
    TT.store(pos, 0, best_score, best_move, flag);
    return alpha;
}

// Negamax search with alpha-beta pruning.
template<bool root_node>
Value negamax(Position& pos, SearchInfo& search_info, Value alpha, Value beta, int depth) {
    int ply = search_info.depth;
    search_info.nodes++;
    search_info.seldepth = std::max(search_info.seldepth, ply);
    search_info.pv_length[ply] = 0;
    if(search_info.is_repetition(pos.hashkey()) || pos.rule50_ply() >= 100) {
        return VALUE_DRAW;
    }
    if(search_info.time_out()) {
        return VALUE_NONE;
    }
    if(search_info.depth > MAX_DEPTH - 1) {
        return evaluate(pos);
    }
    if(depth <= 0) {
        return qsearch(pos, search_info, alpha, beta);
    }
    bool pv_node = (beta - alpha > 1) || root_node;
    // Check for transposition.
    TTEntry entry = TT.get(pos);
    bool tt_hit = entry.key == pos.hashkey();
    if(tt_hit && !pv_node) {
        if(entry.depth >= depth) {
            if(entry.flag == TTFlag::TT_EXACT) {
                return entry.score;
            }
            if(entry.flag == TTFlag::TT_ALPHA && entry.score <= alpha) {
                return entry.score;
            }
            if(entry.flag == TTFlag::TT_BETA && entry.score >= beta) {
                return entry.score;
            }
        }
    }
    MoveList movelist;
    generate_moves<GenType::ALL>(pos, movelist);
    sort_moves(pos, movelist);
    Value best_score = -VALUE_INF;
    Move best_move = MOVE_NONE;
    TTFlag flag = TTFlag::TT_ALPHA;
    int legal_moves = 0;
    for(Move m : movelist) {
        UndoInfo info;
        search_info.store_history(pos.hashkey());
        search_info.depth++;
        if(!pos.make_move(m, info)) {
            pos.unmake_move(info);
            search_info.depth--;
            continue;
        }
        legal_moves++;
        Value score = -negamax<false>(pos, search_info, -beta, -alpha, depth - 1);
        pos.unmake_move(info);
        search_info.depth--;
        if(score > best_score) {
            best_score = score;
            best_move = m;
            if(score > alpha) {
                alpha = score;
                flag = TTFlag::TT_EXACT;
                if(pv_node) {
                    search_info.insert_pv(ply, m);
                }
                if(alpha >= beta) {
                    flag = TTFlag::TT_BETA;
                    break;
                }
            }
        }
    }
    if(legal_moves == 0) {
        // Checkmate or Stalemate.
        return pos.in_check() ? mated_in(ply) : VALUE_DRAW;
    }
    TT.store(pos, depth, best_score, best_move, flag);
    return alpha;
}

void search(Position& pos, SearchInfo& search_info, const Book& book) {
    // Search for book move.
    Move best_move = book.book_move(pos);
    if(best_move != MOVE_NONE) {
        std::cout << "info book move" << std::endl;
        std::cout << "bestmove " << best_move.to_string() << std::endl;
        return;
    }
    // Iterative deepening
    for(int depth = 1; depth <= search_info.max_depth; depth++) {
        Value score = negamax<true>(pos, search_info, -VALUE_INF, VALUE_INF, depth);
        if(search_info.time_out()) {
            break;
        }
        best_move = search_info.pv[0][0];
        std::uint64_t ms = time_elapsed(search_info.start_time);
        std::cout << "info depth " << depth << " seldepth " << search_info.seldepth;
        std::cout << " score cp " << score << " nodes " << search_info.nodes;
        std::cout << " nps " << (search_info.nodes * 1000) / (ms + 1);
        std::cout << " hashfull " << TT.hashfull();
        std::cout << " time " << ms;
        std::cout << " pv " << search_info.pv_to_string() << std::endl;
    }
    std::cout << "bestmove " << best_move.to_string() << std::endl;
}

} // namespace sonic