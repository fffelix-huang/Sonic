#pragma once

#include <cstdint>

#include "bitboard.h"
#include "color.h"
#include "consts.h"
#include "move.h"

namespace sonic {

class Castling {
   public:
    static constexpr Bitboard WHITE_00_PATH_BB   = Bitboard() + SQ_E1 + SQ_F1 + SQ_G1;
    static constexpr Bitboard WHITE_000_PATH_BB  = Bitboard() + SQ_C1 + SQ_D1 + SQ_E1;
    static constexpr Bitboard BLACK_00_PATH_BB   = Bitboard() + SQ_E8 + SQ_F8 + SQ_G8;
    static constexpr Bitboard BLACK_000_PATH_BB  = Bitboard() + SQ_C8 + SQ_D8 + SQ_E8;
    static constexpr Square   WHITE_000_EXTRA_SQ = SQ_B1;
    static constexpr Square   BLACK_000_EXTRA_SQ = SQ_B8;

    static constexpr Move WHITE_00_MOVE  = Move(SQ_E1, SQ_G1);
    static constexpr Move WHITE_000_MOVE = Move(SQ_E1, SQ_C1);
    static constexpr Move BLACK_00_MOVE  = Move(SQ_E8, SQ_G8);
    static constexpr Move BLACK_000_MOVE = Move(SQ_E8, SQ_C8);

    static constexpr Move WHITE_00_ROOK_MOVE  = Move(SQ_H1, SQ_F1);
    static constexpr Move WHITE_000_ROOK_MOVE = Move(SQ_A1, SQ_D1);
    static constexpr Move BLACK_00_ROOK_MOVE  = Move(SQ_H8, SQ_F8);
    static constexpr Move BLACK_000_ROOK_MOVE = Move(SQ_A8, SQ_D8);

    Castling() = default;

    constexpr void set_00(Color c) { data |= (c == Color::WHITE ? 1 : 4); }
    constexpr void set_000(Color c) { data |= (c == Color::WHITE ? 2 : 8); }
    constexpr void set_white_00() { set_00(Color::WHITE); }
    constexpr void set_white_000() { set_000(Color::WHITE); }
    constexpr void set_black_00() { set_00(Color::BLACK); }
    constexpr void set_black_000() { set_000(Color::BLACK); }
    constexpr void set_all() { data = 15; }

    constexpr void reset_00(Color c) { data &= ~(c == Color::WHITE ? 1 : 4); }
    constexpr void reset_000(Color c) { data &= ~(c == Color::WHITE ? 2 : 8); }
    constexpr void reset_white_00() { reset_00(Color::WHITE); }
    constexpr void reset_white_000() { reset_000(Color::WHITE); }
    constexpr void reset_black_00() { reset_00(Color::BLACK); }
    constexpr void reset_black_000() { reset_000(Color::BLACK); }
    constexpr void reset_all() { data = 0; }

    constexpr bool can_00(Color c) const { return (data >> (c == Color::WHITE ? 0 : 2)) & 1; }
    constexpr bool can_000(Color c) const { return (data >> (c == Color::WHITE ? 1 : 3)) & 1; }
    constexpr bool white_can_00() const { return can_00(Color::WHITE); }
    constexpr bool white_can_000() const { return can_000(Color::WHITE); }
    constexpr bool black_can_00() const { return can_00(Color::BLACK); }
    constexpr bool black_can_000() const { return can_000(Color::BLACK); }

    constexpr bool any() const { return data != 0; }
    constexpr bool any(Color c) const { return (data & (c == Color::WHITE ? 3 : 12)) != 0; }

    std::string to_string() const {
        if (!any()) {
            return "-";
        }
        std::string res;
        if (white_can_00()) {
            res += "K";
        }
        if (white_can_000()) {
            res += "Q";
        }
        if (black_can_00()) {
            res += "k";
        }
        if (black_can_000()) {
            res += "q";
        }
        return res;
    }

   private:
    // Bit 0: White 00
    // Bit 1: White 000
    // Bit 2: Black 00
    // Bit 3: Black 000
    std::uint8_t data = 0;
};

} // namespace sonic