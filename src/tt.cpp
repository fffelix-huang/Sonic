#include "tt.h"

#include <cstdint>

#include "chess/all.h"
#include "types.h"

namespace sonic {

TranspositionTable TT;

void TranspositionTable::resize(std::size_t mbSize) {
    constexpr int MB = 1024 * 1024;
    std::size_t new_size = 1;
    // Size must be power of 2.
    while(new_size * 2 * sizeof(TTEntry) <= mbSize * MB) {
        new_size *= 2;
    }
    if(new_size != size) {
        size = new_size;
        entries.resize(size);
        clear();
    }
}

void TranspositionTable::clear() {
    std::fill(entries.begin(), entries.end(), TTEntry());
    entries_count = 0;
}

TTEntry TranspositionTable::get(const Position& pos) const {
    int index = pos.hashkey() & (size - 1);
    return entries[index];
}

void TranspositionTable::store(const Position& pos, int depth, Value score, Move move, TTFlag flag) {
    int index = pos.hashkey() & (size - 1);
    assert(index < size);
    TTEntry& entry = entries[index];
    bool replace = entry.key != pos.hashkey() || entry.depth < depth + 2 || flag == TTFlag::TT_EXACT;
    if(!replace) {
        return;
    }
    if(entry.key == 0) {
        entries_count++;
    }
    entry.key = pos.hashkey();
    entry.depth = depth;
    entry.score = score;
    entry.move = move;
    entry.flag = flag;
}

} // namespace sonic