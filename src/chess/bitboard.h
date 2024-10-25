#pragma once

#include <cstdint>
#include <sstream>
#include <string>

#include "../types.h"
#include "../utils/bits.h"

namespace sonic {

enum File : std::uint8_t {
    FILE_A = 0,
    FILE_B = 1,
    FILE_C = 2,
    FILE_D = 3,
    FILE_E = 4,
    FILE_F = 5,
    FILE_G = 6,
    FILE_H = 7
};

enum Rank : std::uint8_t {
    RANK_1 = 0,
    RANK_2 = 1,
    RANK_3 = 2,
    RANK_4 = 3,
    RANK_5 = 4,
    RANK_6 = 5,
    RANK_7 = 6,
    RANK_8 = 7
};

// clang-format off
#define ENABLE_INCR_DECR_OPERATORS(T) \
    constexpr T& operator++(T& d) { return d = T(std::uint8_t(d) + 1); } \
    constexpr T& operator--(T& d) { return d = T(std::uint8_t(d) - 1); } \
    constexpr T operator++(T& d, int) { T tmp = d; ++d; return tmp; } \
    constexpr T operator--(T& d, int) { T tmp = d; --d; return tmp; } \
// clang-format off

ENABLE_INCR_DECR_OPERATORS(File)
ENABLE_INCR_DECR_OPERATORS(Rank)

#undef ENABLE_INCR_DECR_OPERATORS

class Square {
   public:
    constexpr Square() {}
    constexpr Square(std::uint8_t num) :
        square(num) {}
    constexpr Square(int row, int col) :
        square(row * 8 + col) {}
    constexpr Square(Rank r, File f) :
        Square(f, r) {}
    constexpr Square(File f, Rank r) :
        square(static_cast<std::uint8_t>(r) * 8 + static_cast<std::uint8_t>(f)) {}

    static constexpr int SQ_NB = 64;

    constexpr std::uint8_t  to_int() const { return square; }
    constexpr std::uint64_t to_bb() const { return std::uint64_t(1) << square; }

    std::string to_string() const {
        // SQ_NONE
        if (square == 255) {
            return std::string("(none)");
        }
        return std::string(1, 'a' + col()) + std::string(1, '1' + row());
    }

    constexpr int row() const { return square / 8; }
    constexpr int col() const { return square % 8; }

    constexpr Rank rank() const { return Rank(row()); }
    constexpr File file() const { return File(col()); }

    static bool is_valid(int row, int col) { return row >= 0 && row < 8 && col >= 0 && col < 8; }

    static bool is_valid(Square sq) { return is_valid(sq.row(), sq.col()); }

    constexpr Square& operator+=(const Direction& d) {
        square += int(d);
        return *this;
    }

    friend constexpr Square operator+(Square sq, const Direction& d) { return sq += d; }

    constexpr bool operator==(const Square& other) const { return square == other.square; }

    constexpr bool operator!=(const Square& other) const { return square != other.square; }

   private:
    std::uint8_t square = 0;
};

class Bitboard {
   public:
    constexpr Bitboard() {}
    constexpr Bitboard(std::uint64_t bb) :
        board(bb) {}

    constexpr std::uint64_t to_int() const { return board; }

    constexpr int count() const { return popcount(board); }

    // Set the given square to 1.
    void set(Square sq) { set(sq.to_int()); }
    void set(std::uint8_t pos) { board |= std::uint64_t(1) << pos; }
    void set(int row, int col) { set(Square(row, col)); }

    // Set the given square to 0.
    void reset(Square sq) { reset(sq.to_int()); }
    void reset(std::uint8_t pos) { board &= ~(std::uint64_t(1) << pos); }
    void reset(int row, int col) { reset(Square(row, col)); }

    // Get the value of a square.
    bool get(Square sq) const { return get(sq.to_int()); }
    bool get(std::uint8_t pos) const { return (board >> pos) & 1; }
    bool get(int row, int col) const { return get(Square(row, col)); }

    constexpr bool empty() const { return board == 0; }
    constexpr bool any() const { return board != 0; }

    std::string to_string() const {
        std::ostringstream os;
        for (int row = 7; row >= 0; row--) {
            for (int col = 0; col < 8; col++) {
                os << ".X"[get(row, col)];
            }
            os << "\n";
        }
        return os.str();
    }

    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Square;
        using pointer           = Square*;
        using reference         = Square&;

        iterator(std::uint64_t bb) :
            bitboard(bb),
            sq(lsb(bb)) {}

        iterator& operator++() {
            bitboard &= (bitboard - 1);
            sq = Square(lsb(bitboard));
            return *this;
        }

        iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        reference operator*() { return sq; }

        pointer operator->() { return &sq; }

        bool operator==(const iterator& other) const { return bitboard == other.bitboard; }
        bool operator!=(const iterator& other) const { return bitboard != other.bitboard; }

        std::uint64_t bitboard;
        Square        sq;
    };

    iterator begin() const { return iterator(board); }
    iterator end() const { return iterator(0); }

    constexpr bool operator==(const Bitboard& other) const { return board == other.board; }

    constexpr bool operator!=(const Bitboard& other) const { return board != other.board; }

    constexpr Bitboard& operator&=(const Bitboard& bb) {
        board &= bb.board;
        return *this;
    }

    friend constexpr Bitboard operator&(const Bitboard& a, const Bitboard& b) {
        return Bitboard(a.board & b.board);
    }

    friend constexpr Bitboard operator|(const Bitboard& a, const Bitboard& b) {
        return Bitboard(a.board | b.board);
    }

    friend constexpr Bitboard operator+(const Bitboard& a, const Square& b) {
        return Bitboard(a.board | b.to_bb());
    }

    friend constexpr Bitboard operator-(const Bitboard& a, const Square& b) {
        return Bitboard(a.board & ~b.to_bb());
    }

    constexpr Bitboard& operator+=(const Bitboard& b) {
        board |= b.board;
        return *this;
    }

    constexpr Bitboard& operator+=(const Square& b) {
        board |= std::uint64_t(1) << b.to_int();
        return *this;
    }

    constexpr Bitboard& operator-=(const Bitboard& b) {
        board &= ~b.board;
        return *this;
    }

    constexpr Bitboard& operator-=(const Square& b) {
        board &= ~(std::uint64_t(1) << b.to_int());
        return *this;
    }

    friend constexpr Bitboard operator+(const Bitboard& a, const Bitboard& b) {
        return Bitboard(a.board | b.board);
    }

    friend constexpr Bitboard operator-(const Bitboard& a, const Bitboard& b) {
        return Bitboard(a.board & ~b.board);
    }

   private:
    std::uint64_t board = 0;
};

} // namespace sonic