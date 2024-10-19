#pragma once

#include <cstdint>

#include "bitboard.h"
#include "move.h"

namespace sonic {

const std::string INITIAL_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr Move MOVE_NONE = Move(0);

constexpr Square SQ_NONE = Square(255);

constexpr Square SQ_A1 = Square(0);
constexpr Square SQ_B1 = Square(1);
constexpr Square SQ_C1 = Square(2);
constexpr Square SQ_D1 = Square(3);
constexpr Square SQ_E1 = Square(4);
constexpr Square SQ_F1 = Square(5);
constexpr Square SQ_G1 = Square(6);
constexpr Square SQ_H1 = Square(7);

constexpr Square SQ_A2 = Square(0 + 8);
constexpr Square SQ_B2 = Square(1 + 8);
constexpr Square SQ_C2 = Square(2 + 8);
constexpr Square SQ_D2 = Square(3 + 8);
constexpr Square SQ_E2 = Square(4 + 8);
constexpr Square SQ_F2 = Square(5 + 8);
constexpr Square SQ_G2 = Square(6 + 8);
constexpr Square SQ_H2 = Square(7 + 8);

constexpr Square SQ_A3 = Square(0 + 16);
constexpr Square SQ_B3 = Square(1 + 16);
constexpr Square SQ_C3 = Square(2 + 16);
constexpr Square SQ_D3 = Square(3 + 16);
constexpr Square SQ_E3 = Square(4 + 16);
constexpr Square SQ_F3 = Square(5 + 16);
constexpr Square SQ_G3 = Square(6 + 16);
constexpr Square SQ_H3 = Square(7 + 16);

constexpr Square SQ_A4 = Square(0 + 24);
constexpr Square SQ_B4 = Square(1 + 24);
constexpr Square SQ_C4 = Square(2 + 24);
constexpr Square SQ_D4 = Square(3 + 24);
constexpr Square SQ_E4 = Square(4 + 24);
constexpr Square SQ_F4 = Square(5 + 24);
constexpr Square SQ_G4 = Square(6 + 24);
constexpr Square SQ_H4 = Square(7 + 24);

constexpr Square SQ_A5 = Square(0 + 32);
constexpr Square SQ_B5 = Square(1 + 32);
constexpr Square SQ_C5 = Square(2 + 32);
constexpr Square SQ_D5 = Square(3 + 32);
constexpr Square SQ_E5 = Square(4 + 32);
constexpr Square SQ_F5 = Square(5 + 32);
constexpr Square SQ_G5 = Square(6 + 32);
constexpr Square SQ_H5 = Square(7 + 32);

constexpr Square SQ_A6 = Square(0 + 40);
constexpr Square SQ_B6 = Square(1 + 40);
constexpr Square SQ_C6 = Square(2 + 40);
constexpr Square SQ_D6 = Square(3 + 40);
constexpr Square SQ_E6 = Square(4 + 40);
constexpr Square SQ_F6 = Square(5 + 40);
constexpr Square SQ_G6 = Square(6 + 40);
constexpr Square SQ_H6 = Square(7 + 40);

constexpr Square SQ_A7 = Square(0 + 48);
constexpr Square SQ_B7 = Square(1 + 48);
constexpr Square SQ_C7 = Square(2 + 48);
constexpr Square SQ_D7 = Square(3 + 48);
constexpr Square SQ_E7 = Square(4 + 48);
constexpr Square SQ_F7 = Square(5 + 48);
constexpr Square SQ_G7 = Square(6 + 48);
constexpr Square SQ_H7 = Square(7 + 48);

constexpr Square SQ_A8 = Square(0 + 56);
constexpr Square SQ_B8 = Square(1 + 56);
constexpr Square SQ_C8 = Square(2 + 56);
constexpr Square SQ_D8 = Square(3 + 56);
constexpr Square SQ_E8 = Square(4 + 56);
constexpr Square SQ_F8 = Square(5 + 56);
constexpr Square SQ_G8 = Square(6 + 56);
constexpr Square SQ_H8 = Square(7 + 56);

}  // namespace sonic