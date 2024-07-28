#include "position.h"

#include <cassert>
#include <iomanip>
#include <vector>
#include <sstream>
#include <string>

#include "../utils/strings.h"
#include "attacks.h"
#include "bitboard.h"
#include "castling.h"
#include "color.h"
#include "consts.h"
#include "move.h"
#include "piece.h"
#include "zobrist.h"

namespace sonic {

void Position::set(std::string fen) {
    std::vector<std::string> tokens = split_string(fen, ' ');
    assert(tokens.size() == 6);
    key = 0;
    history_keys.fill(0);
    history_count = 0;

    // 1. Piece placement
    clear_board();
    Square sq = SQ_A8;
    for(char c : tokens[0]) {
        if(isdigit(c)) {
            sq += (c - '0') * Direction::EAST;
        } else if(c == '/') {
            sq += 8 * Direction::WEST;
            sq += Direction::SOUTH;
        } else {
            Piece p = Piece::NO_PIECE;
            switch(c) {
                case 'P': p = Piece::W_PAWN; break;
                case 'R': p = Piece::W_ROOK; break;
                case 'N': p = Piece::W_KNIGHT; break;
                case 'B': p = Piece::W_BISHOP; break;
                case 'Q': p = Piece::W_QUEEN; break;
                case 'K': p = Piece::W_KING; break;

                case 'p': p = Piece::B_PAWN; break;
                case 'r': p = Piece::B_ROOK; break;
                case 'n': p = Piece::B_KNIGHT; break;
                case 'b': p = Piece::B_BISHOP; break;
                case 'q': p = Piece::B_QUEEN; break;
                case 'k': p = Piece::B_KING; break;
            }
            assert(p != Piece::NO_PIECE);
            add_piece(sq, p);
            sq += Direction::EAST;
        }
    }

    // 2. Active color
    sideToMove = (tokens[1] == "w" ? Color::WHITE : Color::BLACK);
    key ^= zobrist_key(sideToMove);

    // 3. Castling rights
    castlings.reset_all();
    for(char c : tokens[2]) {
        if(c == 'K') {
            castlings.set_white_00();
        } else if(c == 'Q') {
            castlings.set_white_000();
        } else if(c == 'k') {
            castlings.set_black_00();
        } else if(c == 'q') {
            castlings.set_black_000();
        }
    }
    key ^= zobrist_key(castlings);

    // 4. En passant square
    enPassant = (tokens[3] == "-" ? SQ_NONE : Square(tokens[3][1] - '1', tokens[3][0] - 'a'));
    key ^= (has_en_passant_capture() * zobrist_key(enPassant));

    // 5-6. ply
    rule50 = std::stoi(tokens[4]);
    gamePly = std::max(2 * std::stoi(tokens[5]) - 2, 0) + (sideToMove == Color::BLACK);
}

// Returns the FEN representation of the position as a string.
std::string Position::fen() const {
    std::ostringstream os;
    Square sq = SQ_A8;
    for(int i = 0; i < 8; i++) {
        int empty_count = 0;
        for(int j = 0; j < 8; j++) {
            Piece p = piece_on(sq);
            if(p == Piece::NO_PIECE) {
                empty_count++;
            } else {
                if(empty_count > 0) {
                    os << empty_count;
                    empty_count = 0;
                }
                os << to_char(p);
            }
            sq += Direction::EAST;
        }
        if(empty_count > 0) {
            os << empty_count;
        }
        sq += 8 * Direction::WEST;
        sq += Direction::SOUTH;
        if(i + 1 < 8) {
            os << "/";
        }
    }
    os << (sideToMove == Color::WHITE ? " w " : " b ");
    os << castlings.to_string();
    os << (enPassant == SQ_NONE ? " - " : " " + enPassant.to_string() + " ");
    os << rule50 << " " << 1 + (gamePly - (sideToMove == Color::BLACK)) / 2;
    return os.str();
}

// Returns if `sq` is being attacked by `c`. Doesn't consider en passant.
bool Position::attacks_by(Square sq, Color c) const {
    // Check knight attacks
    if((knight_attacks[sq.to_int()] & pieces(c, PieceType::KNIGHT)).any()) {
        return true;
    }
    const Bitboard& c_pieces = pieces(c);
    const Bitboard& all_pieces = c_pieces | pieces(other_color(c));
    // Check rook + queen attacks
    const Bitboard& c_rook_and_queen = pieces(c, PieceType::ROOK) | pieces(c, PieceType::QUEEN);
    const Bitboard& rook_attacks = rook_magics[sq.to_int()](all_pieces);
    if((rook_attacks & c_rook_and_queen).any()) {
        return true;
    }
    // Check bishop + queen attacks
    const Bitboard& c_bishop_and_queen = pieces(c, PieceType::BISHOP) | pieces(c, PieceType::QUEEN);
    const Bitboard& bishop_attacks = bishop_magics[sq.to_int()](all_pieces);
    if((bishop_attacks & c_bishop_and_queen).any()) {
        return true;
    }
    // Check pawn attacks
    const Bitboard& c_pawns = pieces(c, PieceType::PAWN);
    if((pawn_attacks[other_color(c)][sq.to_int()] & c_pawns).any()) {
        return true;
    }
    // Check king attacks
    const Bitboard& c_king = pieces(c, PieceType::KING);
    if((king_attacks[sq.to_int()] & c_king).any()) {
        return true;
    }
    return false;
}

// Apply a move on the board and returns true if the given move is legal.
bool Position::make_move(Move m, UndoInfo& info) {
    info.last_move = m;
    info.rule50 = rule50;
    info.history_count = history_count;
    info.castling_state = castlings;
    info.en_passant = enPassant;
    info.captured_piece = Piece::NO_PIECE;
    info.key = key;

    history_keys[history_count] = key;
    history_count++;
    gamePly++;
    rule50++;
    key ^= zobrist_key(castlings) ^ zobrist_key(sideToMove) ^ (has_en_passant_capture() * zobrist_key(enPassant));

    Color us = sideToMove;
    Color them = other_color(sideToMove);
    sideToMove = them;

    Square from = m.from();
    Square to = m.to();
    Piece p = piece_on(from);
    PieceType pt = type(p);

    assert(to != king_square(them));

    // Reset castlings when rook moves or rook was captured.
    if(from == SQ_H1 || to == SQ_H1) {
        castlings.reset_white_00();
    }
    if(from == SQ_A1 || to == SQ_A1) {
        castlings.reset_white_000();
    }
    if(from == SQ_H8 || to == SQ_H8) {
        castlings.reset_black_00();
    }
    if(from == SQ_A8 || to == SQ_A8) {
        castlings.reset_black_000();
    }

    if(pt == PieceType::KING) {
        castlings.reset_00(us);
        castlings.reset_000(us);
    }

    if(piece_on(to) != Piece::NO_PIECE) {
        // Reset rule50 when captures.
        info.captured_piece = piece_on(to);
        rule50 = 0;
        history_count = 0;
        remove_piece(to);
    }

    remove_piece(from);
    add_piece(to, p);

    if(pt == PieceType::PAWN) {
        rule50 = 0;
        history_count = 0;

        // Check if the move is an en passant capture.
        if(to == enPassant) {
            remove_piece(enPassant + (us == Color::WHITE ? Direction::SOUTH : Direction::NORTH));
        }

        // Update en passant square.
        enPassant = SQ_NONE;
        if(from.rank() == Rank::RANK_2 && to.rank() == Rank::RANK_4) {
            enPassant = Square(from.file(), Rank::RANK_3);
        }
        if(from.rank() == Rank::RANK_7 && to.rank() == Rank::RANK_5) {
            enPassant = Square(from.file(), Rank::RANK_6);
        }

        // Handle promotion.
        auto promotion = m.promotion();
        if(promotion != Move::Promotion::None) {
            remove_piece(to);
            Piece promotion_to = Piece::NO_PIECE;
            switch(promotion) {
                case Move::Promotion::Queen: promotion_to = (us == Color::WHITE ? Piece::W_QUEEN : Piece::B_QUEEN); break;
                case Move::Promotion::Knight: promotion_to = (us == Color::WHITE ? Piece::W_KNIGHT : Piece::B_KNIGHT); break;
                case Move::Promotion::Rook: promotion_to = (us == Color::WHITE ? Piece::W_ROOK : Piece::B_ROOK); break;
                case Move::Promotion::Bishop: promotion_to = (us == Color::WHITE ? Piece::W_BISHOP : Piece::B_BISHOP); break;
                default: break;
            }
            add_piece(to, promotion_to);
        }
    } else {
        enPassant = SQ_NONE;
    }
    
    if(pt == PieceType::KING) {
        // Check if the move is castling.
        if(m == Castling::WHITE_00_MOVE) {
            const Move& rook_move = Castling::WHITE_00_ROOK_MOVE;
            remove_piece(rook_move.from());
            add_piece(rook_move.to(), Piece::W_ROOK);
        } else if(m == Castling::WHITE_000_MOVE) {
            const Move& rook_move = Castling::WHITE_000_ROOK_MOVE;
            remove_piece(rook_move.from());
            add_piece(rook_move.to(), Piece::W_ROOK);
        } else if(m == Castling::BLACK_00_MOVE) {
            const Move& rook_move = Castling::BLACK_00_ROOK_MOVE;
            remove_piece(rook_move.from());
            add_piece(rook_move.to(), Piece::B_ROOK);
        } else if(m == Castling::BLACK_000_MOVE) {
            const Move& rook_move = Castling::BLACK_000_ROOK_MOVE;
            remove_piece(rook_move.from());
            add_piece(rook_move.to(), Piece::B_ROOK);
        }
    }

    key ^= zobrist_key(castlings) ^ zobrist_key(sideToMove) ^ (has_en_passant_capture() * zobrist_key(enPassant));

    return !attacks_by(king_square(us), them);
}

void Position::unmake_move(const UndoInfo& info) {
    Piece moved_piece = piece_on(info.last_move.to());
    remove_piece(info.last_move.to());

    // Check promotion.
    if(info.last_move.promotion() != Move::Promotion::None) {
        moved_piece = (sideToMove == Color::WHITE ? Piece::B_PAWN : Piece::W_PAWN);
    }
    add_piece(info.last_move.from(), moved_piece);

    // Check captures.
    if(info.captured_piece != Piece::NO_PIECE) {
        add_piece(info.last_move.to(), info.captured_piece);
    }

    // Check en passant.
    if(type(moved_piece) == PieceType::PAWN && info.last_move.to() == info.en_passant) {
        Direction d = (sideToMove == Color::WHITE ? Direction::NORTH : Direction::SOUTH);
        add_piece(info.last_move.to() + d, sideToMove == Color::WHITE ? Piece::W_PAWN : Piece::B_PAWN);
    }

    // Check castling.
    if(type(moved_piece) == PieceType::KING) {
        if(info.last_move == Castling::WHITE_00_MOVE) {
            const Move& rook_move = Castling::WHITE_00_ROOK_MOVE;
            remove_piece(rook_move.to());
            add_piece(rook_move.from(), Piece::W_ROOK);
        } else if(info.last_move == Castling::WHITE_000_MOVE) {
            const Move& rook_move = Castling::WHITE_000_ROOK_MOVE;
            remove_piece(rook_move.to());
            add_piece(rook_move.from(), Piece::W_ROOK);
        } else if(info.last_move == Castling::BLACK_00_MOVE) {
            const Move& rook_move = Castling::BLACK_00_ROOK_MOVE;
            remove_piece(rook_move.to());
            add_piece(rook_move.from(), Piece::B_ROOK);
        } else if(info.last_move == Castling::BLACK_000_MOVE) {
            const Move& rook_move = Castling::BLACK_000_ROOK_MOVE;
            remove_piece(rook_move.to());
            add_piece(rook_move.from(), Piece::B_ROOK);
        }
    }

    // Restore states.
    castlings = info.castling_state;
    enPassant = info.en_passant;
    rule50 = info.rule50;
    history_count = info.history_count;
    key = info.key;
    gamePly--;
    sideToMove = other_color(sideToMove);
}

void Position::make_null_move(UndoInfo& info) {
    info.last_move = MOVE_NONE;
    info.rule50 = rule50;
    info.history_count = history_count;
    info.castling_state = castlings;
    info.en_passant = enPassant;
    info.captured_piece = Piece::NO_PIECE;
    info.key = key;
    history_keys[history_count] = key;
    history_count++;
    rule50++;
    gamePly++;

    key ^= zobrist_key(sideToMove);
    sideToMove = other_color(sideToMove);
    key ^= zobrist_key(sideToMove);

    key ^= has_en_passant_capture() * zobrist_key(enPassant);
    enPassant = SQ_NONE;
    key ^= has_en_passant_capture() * zobrist_key(enPassant);
}

void Position::unmake_null_move(const UndoInfo& info) {
    rule50 = info.rule50;
    history_count = info.history_count;
    castlings = info.castling_state;
    enPassant = info.en_passant;
    key = info.key;
    gamePly--;
    sideToMove = other_color(sideToMove);
}

// Visualize the current position.
std::string Position::to_string() const {
    std::ostringstream os;
    os << "\n +---+---+---+---+---+---+---+---+\n";
    Square sq = SQ_A8;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            os << " | " << to_char(piece_on(sq));
            sq += Direction::EAST;
        }
        os << " | " << 8 - i << "\n";
        os << " +---+---+---+---+---+---+---+---+\n";
        sq += 8 * Direction::WEST;
        sq += Direction::SOUTH;
    }
    os << "   a   b   c   d   e   f   g   h\n";
    os << "\nFen: " << fen();
    os << "\nKey: " << std::setw(16) << std::setfill('0') << std::hex << std::uppercase << key;
    return os.str();
}

} // namespace sonic