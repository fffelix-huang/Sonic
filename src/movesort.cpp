#include "movesort.h"

#include <algorithm>

namespace sonic {

void sort_moves(const Position& pos, MoveList& movelist) {
    // 1. Promotions
    int promotions = 0;
    for(int i = 0; i < movelist.size(); i++) {
        if(movelist[i].promotion() == Move::Promotion::Queen) {
            std::swap(movelist[i], movelist[promotions]);
            promotions++;
        }
    }
    // 2. Captures
    auto mid = std::partition(movelist.begin() + promotions, movelist.end(), [&](Move m) {
        return pos.is_capture(m);
    });
    auto capture_move_score = [&](Move m) -> int {
        // Pawn, Knight, Bishop, Rook, Queen
        static constexpr int AttackValues[5] = {50, 35, 30, 20, 10};
        Piece us = pos.piece_on(m.from());
        Piece them = pos.piece_on(m.to());
        return AttackValues[type(us)] - AttackValues[type(them)];
    };
    // 3. MVV-LVA
    std::sort(movelist.begin() + promotions, mid, [&](Move a, Move b) {
        return capture_move_score(a) > capture_move_score(b);
    });
}

} // namespace sonic