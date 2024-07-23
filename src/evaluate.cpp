#include "evaluate.h"

#include "chess/all.h"
#include "utils/bits.h"

namespace sonic {

int evaluate(Position& pos) {
    static constexpr int PieceValues[] = {100, 300, 300, 500, 900};
    int white_score = 0;
    int black_score = 0;
    int i = 0;
    for(PieceType pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        white_score += popcount(pos.pieces(Color::WHITE, pt).to_int()) * PieceValues[i];
        black_score += popcount(pos.pieces(Color::BLACK, pt).to_int()) * PieceValues[i];
        i++;
    }
    return pos.side_to_move() == Color::WHITE ? white_score - black_score : black_score - white_score;
}

} // namespace sonic