#pragma once
#include <iostream>

#include <cassert>
#include <vector>
#include <string>

#include "bitboard.h"
#include "castling.h"
#include "color.h"
#include "consts.h"
#include "move.h"
#include "piece.h"

namespace sonic {

class Position {
public:
	Position() : Position(INITIAL_FEN) {}
	Position(const std::string& fen) { set(fen); }

	void set(std::string fen);

    // Returns the FEN representation of the position as a string.
    std::string fen() const;

    constexpr Color side_to_move() const { return sideToMove; }
    constexpr Square en_passant() const { return enPassant; }
    constexpr int game_ply() const { return gamePly; }
    constexpr Castling castling_rights() const { return castlings; }

    // Returns the square that the king in at.
    constexpr Square king_square(Color c) const {
        std::uint64_t raw = pieces(c, PieceType::KING).to_int();
        return Square(lsb(raw));
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
    bool make_move(Move m);

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
    }

    void remove_piece(Square sq) {
        Piece p = board[sq.to_int()];
        if(p != Piece::NO_PIECE) {
            board[sq.to_int()] = Piece::NO_PIECE;
            pieceBB[color(p)][type(p)] -= sq;
        }
    }

    Color sideToMove;
    Piece board[Square::SQ_NB];
    Bitboard pieceBB[Color::COLOR_NB][PieceType::PIECE_NB];
    int gamePly;
    int rule50;
    Castling castlings;
    Square enPassant;
};

} // namespace sonic