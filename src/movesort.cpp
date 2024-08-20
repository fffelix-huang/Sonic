#include "movesort.h"

#include <algorithm>
#include <cassert>

namespace sonic {

void sort_moves(const Position& pos, MoveList& movelist, Move follow_pv_move) {
    auto move_score = [&](Move m) -> int {
        // 1. PV move
        if(m == follow_pv_move) {
            return 1000000;
        }
        // 2. Promotions
        if(m.promotion() == Move::Promotion::Queen) {
            return 100001;
        }
        if(m.promotion() == Move::Promotion::Knight) {
            return 100000;
        }
        // 3. MVV-LVA
        // Pawn, Knight, Bishop, Rook, Queen, King
        static constexpr int AttackValues[6] = {50, 30, 30, 20, 10, 0};
        Piece from = pos.piece_on(m.from());
        Piece to = pos.piece_on(m.to());
        if(pos.is_capture(m)) {
            return AttackValues[type(from)] - AttackValues[type(to)] + 500;
        }
        // 4. Others
        return -AttackValues[type(from)];
    };
    static int scores[218];
    for(size_t i = 0; i < movelist.size(); i++) {
        scores[i] = move_score(movelist[i]);
    }
    // Selection sort
    for(size_t i = 1; i < movelist.size(); i++) {
        int j = i;
        while(j - 1 >= 0 && scores[j] > scores[j - 1]) {
            std::swap(scores[j], scores[j - 1]);
            std::swap(movelist[j], movelist[j - 1]);
            j--;
        }
    }
}

} // namespace sonic
