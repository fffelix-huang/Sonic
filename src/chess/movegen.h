#pragma once

#include "move.h"
#include "position.h"

namespace sonic {

enum GenType {
    CAPTURE,
    NON_CAPTURE,
    ALL
};

// Generates pseudo legal moves.
// template<GenType Type> void generate_pawn_moves(const Position& pos, MoveList& movelist);
// template<GenType Type> void generate_knight_moves(const Position& pos, MoveList& movelist);
// template<GenType Type> void generate_bishop_moves(const Position& pos, MoveList& movelist);
// template<GenType Type> void generate_rook_moves(const Position& pos, MoveList& movelist);
// template<GenType Type> void generate_queen_moves(const Position& pos, MoveList& movelist);
// template<GenType Type> void generate_king_moves(const Position& pos, MoveList& movelist);
template<GenType Type> void generate_moves(const Position& pos, MoveList& movelist);

} // namespace sonic