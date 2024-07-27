#pragma once

#include <cstdint>
#include <vector>

#include "chess/all.h"
#include "types.h"

namespace sonic {

enum TTFlag : std::uint8_t { TT_NONE, TT_ALPHA, TT_BETA, TT_EXACT };

struct TTEntry {
    std::uint64_t key = 0;
    Value score = VALUE_NONE;
    Move move = MOVE_NONE;
    std::uint8_t depth = 0;
    TTFlag flag = TTFlag::TT_NONE;
};

class TranspositionTable {
public:
    TranspositionTable() = default;

    TranspositionTable(std::size_t mbSize) {
        resize(mbSize);
    }

    void resize(std::size_t mbSize);
    void clear();

    const TTEntry* entry_address(std::uint64_t key) const;

    Value probe(const Position& pos, int ply, int depth, Value alpha, Value beta, Move& m) const;
    void store(const Position& pos, int depth, Value score, Move move, TTFlag flag);

    constexpr int hashfull() const { return entries_count * 1000 / size; }

private:
    std::size_t size = 0;
    std::size_t entries_count = 0;
    std::vector<TTEntry> entries;
};

extern TranspositionTable TT;

} // namespace sonic