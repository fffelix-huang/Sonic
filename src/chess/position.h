#pragma once
#include <iostream>

#include <array>
#include <cassert>
#include <vector>
#include <string>

#include "bitboard.h"
#include "castling.h"
#include "color.h"
#include "consts.h"
#include "move.h"
#include "piece.h"
#include "zobrist.h"

namespace sonic {

struct UndoInfo {
    Move last_move;
    Castling castling_state;
    Square en_passant;
    Piece captured_piece;
    int rule50;
    std::uint64_t key;
};

class Position {
public:
	Position() : Position(INITIAL_FEN) {}
	Position(const std::string& fen) { set(fen); }

	void set(std::string fen);

    // Returns the FEN representation of the position as a string.
    std::string fen() const;

    // Returns the zobrist hash key of the current position.
    constexpr std::uint64_t hashkey() const { return key; }

    constexpr Color side_to_move() const { return sideToMove; }
    constexpr Square en_passant() const { return enPassant; }
    constexpr int game_ply() const { return gamePly; }
    constexpr int rule50_ply() const { return rule50; }
    constexpr Castling castling_rights() const { return castlings; }

    // Returns the square that the king in at.
    constexpr Square king_square(Color c) const {
        std::uint64_t raw = pieces(c, PieceType::KING).to_int();
        return Square(lsb(raw));
    }

    bool in_check() const {
        return attacks_by(king_square(sideToMove), other_color(sideToMove));
    }

    bool is_capture(Move m) const {
        const Square& to = m.to();
        return piece_on(to) != Piece::NO_PIECE || to == enPassant;
    }

    bool has_en_passant_capture() const {
        if(enPassant == SQ_NONE) {
            return false;
        }
        Piece my_pawn = (sideToMove == Color::WHITE ? Piece::W_PAWN : Piece::B_PAWN);
        bool result = false;
        if(enPassant.file() != File::FILE_A) {
            result = result || piece_on(enPassant + Direction::WEST) == my_pawn;
        }
        if(enPassant.file() != File::FILE_H) {
            result = result || piece_on(enPassant + Direction::EAST) == my_pawn;
        }
        return result;
    }

    constexpr Bitboard pieces(Color c) const {
        return pieceBB[c][PieceType::PAWN] | pieceBB[c][PieceType::KNIGHT]
               | pieceBB[c][PieceType::BISHOP] | pieceBB[c][PieceType::ROOK]
               | pieceBB[c][PieceType::QUEEN] | pieceBB[c][PieceType::KING];
    }

    constexpr Bitboard pieces(Color c, PieceType pt) const { return pieceBB[c][pt]; }

    // Returns the piece on `sq`.
    constexpr Piece piece_on(Square sq) const {
        return board[sq.to_int()];
    }

    // Returns if `sq` is being attacked by `c`. Doesn't consider en passant.
    bool attacks_by(Square sq, Color c) const;

    // Apply a move on the board and returns true if the given move is legal.
    bool make_move(Move m, UndoInfo& info);

    // Undo a move.
    void unmake_move(const UndoInfo& info);

    // Check if the current position occurs before.
    bool is_repetition() const {
        for(int i = 2; i < std::min(gamePly, rule50); i += 2) {
            if(history_keys[gamePly - i] == key) {
                return true;
            }
        }
        return false;
    }

    // Visualize the current position.
    std::string to_string() const;

private:
    void clear_board() {
        for(int i = 0; i < Square::SQ_NB; i++) {
            board[i] = Piece::NO_PIECE;
        }
        for(int i = 0; i < Color::COLOR_NB; i++) {
            for(int j = 0; j < PieceType::PIECE_NB; j++) {
                pieceBB[i][j] = Bitboard(0);
            }
        }
    }

    void add_piece(Square sq, Piece p) {
        board[sq.to_int()] = p;
        pieceBB[color(p)][type(p)] += sq;
        key ^= zobrist_key(sq, p);
    }

    void remove_piece(Square sq) {
        Piece p = board[sq.to_int()];
        if(p != Piece::NO_PIECE) {
            board[sq.to_int()] = Piece::NO_PIECE;
            pieceBB[color(p)][type(p)] -= sq;
            key ^= zobrist_key(sq, p);
        }
    }

    Color sideToMove;
    Piece board[Square::SQ_NB];
    Bitboard pieceBB[Color::COLOR_NB][PieceType::PIECE_NB];
    int gamePly;
    int rule50;
    Castling castlings;
    Square enPassant;
    std::uint64_t key;
    std::array<std::uint64_t, MAX_MOVES> history_keys = {};
};

} // namespace sonic