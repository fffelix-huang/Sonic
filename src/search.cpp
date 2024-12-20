#include "search.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>

#include "chess/all.h"
#include "utils/misc.h"
#include "utils/timer.h"
#include "book.h"
#include "evaluate.h"
#include "movesort.h"
#include "tt.h"
#include "tune.h"
#include "types.h"
#include "uci.h"

namespace sonic {

Value qsearch(Position& pos, SearchInfo& search_info, Value alpha, Value beta) {
    int ply = search_info.depth;
    search_info.nodes++;
    search_info.seldepth       = std::max(search_info.seldepth, ply);
    search_info.pv_length[ply] = 0;
    if (pos.is_draw()) {
        return VALUE_DRAW;
    }
    if (search_info.time_out()) {
        return VALUE_NONE;
    }

    // Check for transposition.
    Move  tt_move  = MOVE_NONE;
    Value tt_score = TT.probe(pos, ply, 0, alpha, beta, tt_move);
    bool  tt_hit   = (tt_score != VALUE_NONE);
    if (ply > 0 && tt_hit) {
        return tt_score;
    }

    Value eval = (tt_hit ? tt_score : evaluate(pos));
    if (ply > MAX_DEPTH - 1) {
        return eval;
    }

    bool in_check = pos.in_check();
    if (!in_check) {
        if (eval >= beta) {
            return eval;
        }
        alpha = std::max(alpha, eval);
    }

    // Delta pruning.
    if (eval + DELTA_MARGIN < alpha) {
        return alpha;
    }

    MoveList captures;
    generate_moves<GenType::CAPTURE>(pos, captures);
    sort_moves(pos, captures, MOVE_NONE);

    Move   best_move = MOVE_NONE;
    TTFlag flag      = TTFlag::TT_ALPHA;
    for (Move m : captures) {
        UndoInfo info;
        search_info.depth++;
        if (!pos.make_move(m, info)) {
            pos.unmake_move(info);
            search_info.depth--;
            continue;
        }
        prefetch(TT.entry_address(pos.hashkey()));
        Value score = -qsearch(pos, search_info, -beta, -alpha);
        pos.unmake_move(info);
        search_info.depth--;
        if (score > alpha) {
            alpha     = score;
            best_move = m;
            flag      = TTFlag::TT_EXACT;
            if (alpha >= beta) {
                flag = TTFlag::TT_BETA;
                break;
            }
            search_info.insert_pv(ply, m);
        }
    }
    TT.store(pos, 0, alpha, best_move, flag);
    return alpha;
}

// Negamax search with alpha-beta pruning.
Value negamax(
    Position& pos, SearchInfo& search_info, Value alpha, Value beta, int depth, bool do_null) {
    int  ply       = search_info.depth;
    bool root_node = (ply == 0);
    search_info.nodes++;
    search_info.seldepth       = std::max(search_info.seldepth, ply);
    search_info.pv_length[ply] = 0;
    if (!root_node && pos.is_draw()) {
        return VALUE_DRAW;
    }
    if (search_info.time_out()) {
        return VALUE_NONE;
    }
    if (ply > MAX_DEPTH - 1) {
        return evaluate(pos);
    }

    // Mate distance pruning.
    if (!root_node) {
        alpha = std::max(alpha, mated_in(ply));
        beta  = std::min(beta, mate_in(ply + 1));
        if (alpha >= beta) {
            return alpha;
        }
    }

    bool pv_node = (beta - alpha > 1);
    // Check for transposition.
    Move  tt_move  = MOVE_NONE;
    Value tt_score = TT.probe(pos, ply, depth, alpha, beta, tt_move);
    bool  tt_hit   = (tt_score != VALUE_NONE);
    if (!root_node && tt_hit && !pv_node) {
        return tt_score;
    }

    // Check extension.
    bool in_check = pos.in_check();
    if (in_check) {
        depth++;
    }
    if (depth <= 0) {
        return qsearch(pos, search_info, alpha, beta);
    }

    Value eval = VALUE_INF;
    if (!in_check) {
        // Use evaluation stored in TT.
        eval = (tt_hit ? tt_score : evaluate(pos));

        // Reverse futility pruning.
        if (depth <= 3 && eval - (RFP_BASE + RFP_MULTIPLIER * depth * depth) >= beta) {
            return (eval + beta) / 2;
        }
    }

    // Null move pruning.
    Color us = pos.side_to_move();
    bool  has_big_piece =
        (pos.pieces(us) - pos.pieces(us, PieceType::KING) - pos.pieces(us, PieceType::PAWN)).any();
    if (do_null && !in_check && has_big_piece && search_info.depth > 0 && depth >= 3) {
        UndoInfo info;
        search_info.depth++;
        pos.make_null_move(info);
        Value null_score = -negamax(pos, search_info, -beta, -beta + 1, depth - 1 - 2, false);
        pos.unmake_null_move(info);
        search_info.depth--;
        if (null_score >= beta) {
            return beta;
        }
    }

    MoveList movelist;
    generate_moves<GenType::ALL>(pos, movelist);
    sort_moves(pos, movelist, tt_move);

    Value  best_score     = -VALUE_INF;
    Move   best_move      = MOVE_NONE;
    TTFlag flag           = TTFlag::TT_ALPHA;
    int    moves_searched = 0;
    for (Move m : movelist) {
        bool     is_quiet = pos.is_quiet(m);
        UndoInfo info;
        search_info.depth++;
        if (!pos.make_move(m, info)) {
            pos.unmake_move(info);
            search_info.depth--;
            continue;
        }
        moves_searched++;
        bool gives_check = pos.in_check();
        if (!root_node) {
            // Futility pruning.
            Value futility_margin = FP_BASE + FP_MULTIPLIER * depth;
            if (!in_check && depth <= 2 && is_quiet && !gives_check
                && eval + futility_margin < alpha) {
                pos.unmake_move(info);
                search_info.depth--;
                continue;
            }
        }
        prefetch(TT.entry_address(pos.hashkey()));
        Value score = VALUE_NONE;
        if (moves_searched >= 5 && depth >= 3 && !in_check) {
            score = -negamax(pos, search_info, -alpha - 1, -alpha, depth - 2, true);
        } else {
            // Do search on full-depth.
            score = VALUE_INF;
        }
        if (score > alpha) {
            // PV search.
            score = -negamax(pos, search_info, -alpha - 1, -alpha, depth - 1, true);
            if (alpha < score && score < beta) {
                score = -negamax(pos, search_info, -beta, -alpha, depth - 1, true);
            }
        }
        pos.unmake_move(info);
        search_info.depth--;
        if (score > best_score) {
            best_score = score;
            best_move  = m;
            if (score > alpha) {
                alpha = score;
                flag  = TTFlag::TT_EXACT;
                if (alpha >= beta) {
                    flag = TTFlag::TT_BETA;
                    break;
                }
                search_info.insert_pv(ply, m);
            }
        }
    }
    if (moves_searched == 0) {
        // Checkmate or Stalemate.
        return in_check ? mated_in(ply) : VALUE_DRAW;
    }
    TT.store(pos, depth, best_score, best_move, flag);
    return alpha;
}

void search(Position& pos, SearchInfo& search_info) {
    // Update TT size.
    TT.resize(int(options["Hash"]));

    // Search for book move.
    Book book(options["Book"]);
    Move best_move = book.book_move(pos);
    if (best_move != MOVE_NONE) {
        std::cout << "info book move" << std::endl;
        std::cout << "bestmove " << best_move.to_string() << std::endl;
        return;
    }
    // Aspiration window.
    Value alpha = -VALUE_INF, beta = VALUE_INF;
    // Iterative deepening.
    for (int depth = 1; depth <= search_info.max_depth; depth++) {
        search_info.follow_pv = true;
        Value score           = negamax(pos, search_info, alpha, beta, depth, true);
        if (search_info.time_out()) {
            break;
        }
        if (score <= alpha || score >= beta) {
            // Research with full window.
            alpha = -VALUE_INF;
            beta  = VALUE_INF;
            depth--;
            continue;
        }
        best_move        = search_info.pv[0][0];
        std::uint64_t ms = time_elapsed(search_info.start_time);
        std::cout << "info depth " << depth << " seldepth " << search_info.seldepth;
        std::cout << " score " << value_to_string(score);
        std::cout << " nodes " << search_info.nodes;
        std::cout << " nps " << (search_info.nodes * 1000) / (ms + 1);
        std::cout << " hashfull " << TT.hashfull();
        std::cout << " time " << ms;
        std::cout << " pv " << search_info.pv_to_string() << std::endl;
        alpha = std::max(score - 20, -VALUE_INF);
        beta  = std::min(score + 20, VALUE_INF);
    }
    std::cout << "bestmove " << best_move.to_string() << std::endl;
}

} // namespace sonic
