#include "movegen.h"

#include <algorithm>

#include "attacks.h"
#include "bitboard.h"
#include "castling.h"
#include "color.h"
#include "move.h"
#include "piece.h"
#include "position.h"

namespace sonic {

template<GenType Type>
void generate_pawn_moves(const Position& pos, MoveList& movelist) {
    const Color&     us             = pos.side_to_move();
    const Bitboard&  other_pieces   = pos.pieces(other_color(us));
    const Rank&      initial_rank   = (us == Color::WHITE ? Rank::RANK_2 : Rank::RANK_7);
    const Rank&      promotion_rank = (us == Color::WHITE ? Rank::RANK_7 : Rank::RANK_2);
    const Direction& pawn_direction = (us == Color::WHITE ? Direction::NORTH : Direction::SOUTH);
    Bitboard         pawns          = pos.pieces(us, PieceType::PAWN);
    for (Square pawn : pawns)
    {
        if (pawn.rank() != promotion_rank)
        {
            if constexpr (Type != GenType::CAPTURE)
            {
                Square one_step = pawn + pawn_direction;
                if (pos.piece_on(one_step) == Piece::NO_PIECE)
                {
                    movelist.emplace_back(pawn, one_step);
                    // Check two steps
                    if (pawn.rank() == initial_rank)
                    {
                        Square two_step = one_step + pawn_direction;
                        if (pos.piece_on(two_step) == Piece::NO_PIECE)
                        {
                            movelist.emplace_back(pawn, two_step);
                        }
                    }
                }
            }
            if constexpr (Type != GenType::NON_CAPTURE)
            {
                for (Square to : pawn_attacks[us][pawn.to_int()])
                {
                    if (other_pieces.get(to) || to == pos.en_passant())
                    {
                        movelist.emplace_back(pawn, to);
                    }
                }
            }
        }
        else
        {
            // Check promotion
            if constexpr (Type != GenType::CAPTURE)
            {
                Square one_step = pawn + pawn_direction;
                if (pos.piece_on(one_step) == Piece::NO_PIECE)
                {
                    for (Move::Promotion promo : {Move::Promotion::Queen, Move::Promotion::Knight,
                                                  Move::Promotion::Rook, Move::Promotion::Bishop})
                    {
                        movelist.emplace_back(pawn, one_step, promo);
                    }
                }
            }
            if constexpr (Type != GenType::NON_CAPTURE)
            {
                for (Square to : pawn_attacks[us][pawn.to_int()])
                {
                    if (other_pieces.get(to))
                    {
                        for (Move::Promotion promo :
                             {Move::Promotion::Queen, Move::Promotion::Knight,
                              Move::Promotion::Rook, Move::Promotion::Bishop})
                        {
                            movelist.emplace_back(pawn, to, promo);
                        }
                    }
                }
            }
        }
    }
}

template<GenType Type>
void generate_knight_moves(const Position& pos, MoveList& movelist) {
    const Bitboard& my_pieces    = pos.pieces(pos.side_to_move());
    const Bitboard& other_pieces = pos.pieces(other_color(pos.side_to_move()));
    Bitboard        knights      = pos.pieces(pos.side_to_move(), PieceType::KNIGHT);
    for (Square knight : knights)
    {
        Bitboard attacks = knight_attacks[knight.to_int()] - my_pieces;
        if constexpr (Type != GenType::ALL)
        {
            if constexpr (Type == GenType::CAPTURE)
            {
                attacks &= other_pieces;
            }
            if constexpr (Type == GenType::NON_CAPTURE)
            {
                attacks -= other_pieces;
            }
        }
        for (Square to : attacks)
        {
            movelist.emplace_back(knight, to);
        }
    }
}

template<GenType Type>
void generate_bishop_moves(const Position& pos, MoveList& movelist) {
    const Bitboard& my_pieces    = pos.pieces(pos.side_to_move());
    const Bitboard& other_pieces = pos.pieces(other_color(pos.side_to_move()));
    const Bitboard& all_pieces   = my_pieces | other_pieces;
    Bitboard        bishops      = pos.pieces(pos.side_to_move(), PieceType::BISHOP);
    for (Square bishop : bishops)
    {
        Bitboard attacks = bishop_magics[bishop.to_int()](all_pieces);
        attacks -= my_pieces;
        if constexpr (Type != GenType::ALL)
        {
            if constexpr (Type == GenType::CAPTURE)
            {
                attacks &= other_pieces;
            }
            if constexpr (Type == GenType::NON_CAPTURE)
            {
                attacks -= other_pieces;
            }
        }
        for (Square to : attacks)
        {
            movelist.emplace_back(bishop, to);
        }
    }
}

template<GenType Type>
void generate_rook_moves(const Position& pos, MoveList& movelist) {
    const Bitboard& my_pieces    = pos.pieces(pos.side_to_move());
    const Bitboard& other_pieces = pos.pieces(other_color(pos.side_to_move()));
    const Bitboard& all_pieces   = my_pieces | other_pieces;
    Bitboard        rooks        = pos.pieces(pos.side_to_move(), PieceType::ROOK);
    for (Square rook : rooks)
    {
        Bitboard attacks = rook_magics[rook.to_int()](all_pieces);
        attacks -= my_pieces;
        if constexpr (Type != GenType::ALL)
        {
            if constexpr (Type == GenType::CAPTURE)
            {
                attacks &= other_pieces;
            }
            if constexpr (Type == GenType::NON_CAPTURE)
            {
                attacks -= other_pieces;
            }
        }
        for (Square to : attacks)
        {
            movelist.emplace_back(rook, to);
        }
    }
}

template<GenType Type>
void generate_queen_moves(const Position& pos, MoveList& movelist) {
    const Bitboard& my_pieces    = pos.pieces(pos.side_to_move());
    const Bitboard& other_pieces = pos.pieces(other_color(pos.side_to_move()));
    const Bitboard& all_pieces   = my_pieces | other_pieces;
    Bitboard        queens       = pos.pieces(pos.side_to_move(), PieceType::QUEEN);
    for (Square queen : queens)
    {
        Bitboard attacks =
          (rook_magics[queen.to_int()](all_pieces) | bishop_magics[queen.to_int()](all_pieces));
        attacks -= my_pieces;
        if constexpr (Type != GenType::ALL)
        {
            if constexpr (Type == GenType::CAPTURE)
            {
                attacks &= other_pieces;
            }
            if constexpr (Type == GenType::NON_CAPTURE)
            {
                attacks -= other_pieces;
            }
        }
        for (Square to : attacks)
        {
            movelist.emplace_back(queen, to);
        }
    }
}

template<GenType Type>
void generate_king_moves(const Position& pos, MoveList& movelist) {
    const Bitboard& my_pieces    = pos.pieces(pos.side_to_move());
    const Bitboard& other_pieces = pos.pieces(other_color(pos.side_to_move()));
    Square          king         = pos.king_square(pos.side_to_move());
    Bitboard        attacks      = king_attacks[king.to_int()] - my_pieces;
    if constexpr (Type != GenType::ALL)
    {
        if constexpr (Type == GenType::CAPTURE)
        {
            attacks &= other_pieces;
        }
        if constexpr (Type == GenType::NON_CAPTURE)
        {
            attacks -= other_pieces;
        }
    }
    for (Square to : attacks)
    {
        movelist.emplace_back(king, to);
    }

    // Check castlings
    if constexpr (Type == GenType::CAPTURE)
    {
        return;
    }
    if (pos.side_to_move() == Color::WHITE)
    {
        // White short castle
        if (pos.castling_rights().white_can_00())
        {
            bool invalid = false;
            for (Square sq : Castling::WHITE_00_PATH_BB)
            {
                if (pos.attacks_by(sq, Color::BLACK))
                {
                    invalid = true;
                    break;
                }
                if (sq != king && pos.piece_on(sq) != Piece::NO_PIECE)
                {
                    invalid = true;
                    break;
                }
            }
            if (!invalid)
            {
                movelist.push_back(Castling::WHITE_00_MOVE);
            }
        }

        // White long castle
        if (pos.castling_rights().white_can_000())
        {
            bool invalid = (pos.piece_on(Castling::WHITE_000_EXTRA_SQ) != Piece::NO_PIECE);
            for (Square sq : Castling::WHITE_000_PATH_BB)
            {
                if (pos.attacks_by(sq, Color::BLACK))
                {
                    invalid = true;
                    break;
                }
                if (sq != king && pos.piece_on(sq) != Piece::NO_PIECE)
                {
                    invalid = true;
                    break;
                }
            }
            if (!invalid)
            {
                movelist.push_back(Castling::WHITE_000_MOVE);
            }
        }
    }
    else
    {
        // Black short castle
        if (pos.castling_rights().black_can_00())
        {
            bool invalid = false;
            for (Square sq : Castling::BLACK_00_PATH_BB)
            {
                if (pos.attacks_by(sq, Color::WHITE))
                {
                    invalid = true;
                    break;
                }
                if (sq != king && pos.piece_on(sq) != Piece::NO_PIECE)
                {
                    invalid = true;
                    break;
                }
            }
            if (!invalid)
            {
                movelist.push_back(Castling::BLACK_00_MOVE);
            }
        }

        // Black long castle
        if (pos.castling_rights().black_can_000())
        {
            bool invalid = (pos.piece_on(Castling::BLACK_000_EXTRA_SQ) != Piece::NO_PIECE);
            for (Square sq : Castling::BLACK_000_PATH_BB)
            {
                if (pos.attacks_by(sq, Color::WHITE))
                {
                    invalid = true;
                    break;
                }
                if (sq != king && pos.piece_on(sq) != Piece::NO_PIECE)
                {
                    invalid = true;
                    break;
                }
            }
            if (!invalid)
            {
                movelist.push_back(Castling::BLACK_000_MOVE);
            }
        }
    }
}

// Generates pseudo legal moves.
template<GenType Type>
void generate_moves(const Position& pos, MoveList& movelist) {
    generate_pawn_moves<Type>(pos, movelist);
    generate_knight_moves<Type>(pos, movelist);
    generate_bishop_moves<Type>(pos, movelist);
    generate_rook_moves<Type>(pos, movelist);
    generate_queen_moves<Type>(pos, movelist);
    generate_king_moves<Type>(pos, movelist);
}

template void generate_moves<GenType::CAPTURE>(const Position& pos, MoveList& movelist);
template void generate_moves<GenType::NON_CAPTURE>(const Position& pos, MoveList& movelist);
template void generate_moves<GenType::ALL>(const Position& pos, MoveList& movelist);

}  // namespace sonic