#include "tt.h"

#include <cstdint>

#include "chess/all.h"
#include "types.h"

namespace sonic {

TranspositionTable TT(16);

void TranspositionTable::resize(std::size_t mbSize) {
    constexpr int MB       = 1024 * 1024;
    std::size_t   new_size = 1;
    // Size must be power of 2.
    while (new_size * 2 * sizeof(TTEntry) <= mbSize * MB) {
        new_size *= 2;
    }
    if (new_size != size) {
        size = new_size;
        entries.resize(size);
        clear();
    }
}

void TranspositionTable::clear() {
    std::fill(entries.begin(), entries.end(), TTEntry());
    entries_count = 0;
}

Value TranspositionTable::probe(
    const Position& pos, int ply, int depth, Value alpha, Value beta, Move& m) const {
    const TTEntry& entry = entries[pos.hashkey() & (size - 1)];
    if (entry.key != pos.hashkey()) {
        return VALUE_NONE;
    }
    m = entry.move;
    if (entry.depth >= depth) {
        Value score = entry.score;
        if (is_mate_value(score)) {
            if (score < 0) {
                score += ply;
            } else {
                score -= ply;
            }
        }
        if (entry.flag == TTFlag::TT_EXACT) {
            return score;
        }
        if (entry.flag == TTFlag::TT_ALPHA && score <= alpha) {
            return alpha;
        }
        if (entry.flag == TTFlag::TT_BETA && score >= beta) {
            return beta;
        }
    }
    return VALUE_NONE;
}

void TranspositionTable::store(
    const Position& pos, int depth, Value score, Move move, TTFlag flag) {
    size_t index = pos.hashkey() & (size - 1);
    assert(index < size);
    TTEntry& entry = entries[index];
    bool     replace =
        entry.key != pos.hashkey() || entry.depth < depth + 2 || flag == TTFlag::TT_EXACT;
    if (!replace) {
        return;
    }
    if (entry.key == 0) {
        entries_count++;
    }
    entry.key   = pos.hashkey();
    entry.depth = depth;
    entry.score = score;
    entry.move  = move;
    entry.flag  = flag;
}

const TTEntry* TranspositionTable::entry_address(std::uint64_t key) const {
    return &entries[key & (size - 1)];
}

} // namespace sonic