#pragma once

#include "chess/all.h"

namespace sonic {

void sort_moves(const Position& pos, MoveList& movelist, Move follow_pv_move);

}  // namespace sonic