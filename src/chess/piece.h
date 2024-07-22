#pragma once

#include <cassert>

#include "color.h"

namespace sonic {

enum PieceType {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    PIECE_NB = 6,
    NO_PIECE_TYPE = 7
};

enum Piece {
    W_PAWN = 0, W_KNIGHT = 1, W_BISHOP = 2,
    W_ROOK = 3, W_QUEEN = 4, W_KING = 5,

    B_PAWN = 0 + 8, B_KNIGHT = 1 + 8, B_BISHOP = 2 + 8,
    B_ROOK = 3 + 8, B_QUEEN = 4 + 8, B_KING = 5 + 8,

    NO_PIECE = 7
};

constexpr Color color(Piece p) {
    assert(p != Piece::NO_PIECE);
    return p < 8 ? Color::WHITE : Color::BLACK;
}

constexpr PieceType type(Piece p) {
    return PieceType(p & 7);
}

constexpr char to_char(Piece p) {
	switch(p) {
		case Piece::W_PAWN:   return 'P';
		case Piece::W_ROOK:   return 'R';
		case Piece::W_KNIGHT: return 'N';
		case Piece::W_BISHOP: return 'B';
		case Piece::W_QUEEN:  return 'Q';
		case Piece::W_KING:   return 'K';

		case Piece::B_PAWN:   return 'p';
		case Piece::B_ROOK:   return 'r';
		case Piece::B_KNIGHT: return 'n';
		case Piece::B_BISHOP: return 'b';
		case Piece::B_QUEEN:  return 'q';
		case Piece::B_KING:   return 'k';

		default: return ' ';
	}
}

} // namespace sonic