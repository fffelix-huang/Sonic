#include "search.h"

#include <algorithm>
#include <chrono>
#include <limits>

#include "chess/all.h"
#include "utils/misc.h"
#include "utils/timer.h"
#include "book.h"
#include "evaluate.h"
#include "movesort.h"
#include "tt.h"
#include "types.h"

namespace sonic {

Value qsearch(Position& pos, SearchInfo& search_info, Value alpha, Value beta) {
    int ply = search_info.depth;
    search_info.nodes++;
    search_info.seldepth = std::max(search_info.seldepth, ply);
    search_info.pv_length[ply] = 0;
    if((ply > 0 && pos.is_repetition()) || pos.rule50_ply() >= 100) {
        return VALUE_DRAW;
    }
    if(search_info.time_out()) {
        return VALUE_NONE;
    }
    // Check for transposition.
    Move tt_move = MOVE_NONE;
    Value tt_score = TT.probe(pos, ply, 0, alpha, beta, tt_move);
    if(ply > 0 && tt_score != VALUE_NONE) {
        return tt_score;
    }
    Value static_eval = evaluate(pos);
    if(ply > MAX_DEPTH - 1) {
        return static_eval;
    }
    bool in_check = pos.in_check();
    if(!in_check) {
        if(static_eval >= beta) {
            return static_eval;
        }
        alpha = std::max(alpha, static_eval);
    }
    MoveList captures;
    generate_moves<GenType::CAPTURE>(pos, captures);
    sort_moves(pos, captures);
    Value best_score = -VALUE_INF;
    Move best_move = MOVE_NONE;
    TTFlag flag = TTFlag::TT_ALPHA;
    for(Move m : captures) {
        UndoInfo info;
        search_info.depth++;
        if(!pos.make_move(m, info)) {
            pos.unmake_move(info);
            search_info.depth--;
            continue;
        }
        prefetch(TT.entry_address(pos.hashkey()));
        Value score = -qsearch(pos, search_info, -beta, -alpha);
        pos.unmake_move(info);
        search_info.depth--;
        if(score > best_score) {
            best_score = score;
            best_move = m;
            if(score > alpha) {
                alpha = score;
                flag = TTFlag::TT_EXACT;
                if(alpha >= beta) {
                    flag = TTFlag::TT_BETA;
                    break;
                }
                search_info.insert_pv(ply, m);
            }
        }
    }
    TT.store(pos, 0, best_score, best_move, flag);
    return alpha;
}

// Negamax search with alpha-beta pruning.
Value negamax(Position& pos, SearchInfo& search_info, Value alpha, Value beta, int depth, bool do_null) {
    int ply = search_info.depth;
    bool root_node = (ply == 0);
    search_info.nodes++;
    search_info.seldepth = std::max(search_info.seldepth, ply);
    search_info.pv_length[ply] = 0;
    if((!root_node && pos.is_repetition()) || pos.rule50_ply() >= 100) {
        return VALUE_DRAW;
    }
    if(search_info.time_out()) {
        return VALUE_NONE;
    }
    if(ply > MAX_DEPTH - 1) {
        return evaluate(pos);
    }

    // Mate distance pruning.
    if(!root_node) {
        alpha = std::max(alpha, mated_in(ply));
        beta = std::min(beta, mate_in(ply + 1));
        if(alpha >= beta) {
            return alpha;
        }
    }

    bool pv_node = (beta - alpha > 1);
    // Check for transposition.
    Move tt_move = MOVE_NONE;
    Value tt_score = TT.probe(pos, ply, depth, alpha, beta, tt_move);
    bool tt_hit = (tt_score != VALUE_NONE);
    if(!root_node && tt_hit && !pv_node) {
        return tt_score;
    }

    // Check extension.
    bool in_check = pos.in_check();
    if(in_check) {
        depth++;
    }
    if(depth <= 0) {
        return qsearch(pos, search_info, alpha, beta);
    }

    Value eval = (in_check ? VALUE_INF : evaluate(pos));

    // Null move pruning.
    Color us = pos.side_to_move();
    bool has_big_piece = (pos.pieces(us) - pos.pieces(us, PieceType::KING) - pos.pieces(us, PieceType::PAWN)).any();
    if(do_null && !in_check && has_big_piece && search_info.depth > 0 && depth >= 3) {
        UndoInfo info;
        search_info.depth++;
        pos.make_null_move(info);
        Value null_score = -negamax(pos, search_info, -beta, -beta + 1, depth - 1 - 2, false);
        pos.unmake_null_move(info);
        search_info.depth--;
        if(null_score >= beta) {
            return beta;
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
        bool is_quiet = pos.is_quiet(m);
        UndoInfo info;
        search_info.depth++;
        if(!pos.make_move(m, info)) {
            pos.unmake_move(info);
            search_info.depth--;
            continue;
        }
        legal_moves++;
        bool gives_check = pos.in_check();
        if(!root_node) {
            // Futility pruning.
            Value futility_margin = 175 + 125 * depth;
            if(!in_check && depth <= 2 && is_quiet && !gives_check && eval + futility_margin < alpha) {
                pos.unmake_move(info);
                search_info.depth--;
                continue;
            }
        }
        prefetch(TT.entry_address(pos.hashkey()));
        // PV search.
        Value score = -negamax(pos, search_info, -alpha - 1, -alpha, depth - 1, true);
        if(alpha < score && score < beta) {
            score = -negamax(pos, search_info, -beta, -alpha, depth - 1, true);
        }
        pos.unmake_move(info);
        search_info.depth--;
        if(score > best_score) {
            best_score = score;
            best_move = m;
            if(score > alpha) {
                alpha = score;
                flag = TTFlag::TT_EXACT;
                if(alpha >= beta) {
                    flag = TTFlag::TT_BETA;
                    break;
                }
                search_info.insert_pv(ply, m);
            }
        }
    }
    if(legal_moves == 0) {
        // Checkmate or Stalemate.
        return in_check ? mated_in(ply) : VALUE_DRAW;
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
    // Aspiration window.
    Value alpha = -VALUE_INF, beta = VALUE_INF;
    // Iterative deepening.
    for(int depth = 1; depth <= search_info.max_depth; depth++) {
        Value score = negamax(pos, search_info, alpha, beta, depth, true);
        if(search_info.time_out()) {
            break;
        }
        if(score <= alpha || score >= beta) {
            // Research with full window.
            alpha = -VALUE_INF;
            beta = VALUE_INF;
            depth--;
            continue;
        }
        best_move = search_info.pv[0][0];
        std::uint64_t ms = time_elapsed(search_info.start_time);
        std::cout << "info depth " << depth << " seldepth " << search_info.seldepth;
        std::cout << " score " << value_to_string(score);
        std::cout << " nodes " << search_info.nodes;
        std::cout << " nps " << (search_info.nodes * 1000) / (ms + 1);
        std::cout << " hashfull " << TT.hashfull();
        std::cout << " time " << ms;
        std::cout << " pv " << search_info.pv_to_string() << std::endl;
        alpha = std::max(score - 20, -VALUE_INF);
        beta = std::min(score + 20, VALUE_INF);
    }
    std::cout << "bestmove " << best_move.to_string() << std::endl;
}

} // namespace sonic
