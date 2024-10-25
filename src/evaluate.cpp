#include "evaluate.h"

#include "chess/all.h"
#include "utils/bits.h"
#include "types.h"

namespace sonic {

constexpr std::pair<Value, Value> PieceSquareTable[PieceType::PIECE_NB][Square::SQ_NB] = {
  {
    // Pawn
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {166, 256}, {192, 256}, {204, 256}, {216, 256}, {216, 256}, {204, 256}, {192, 256}, {166, 256},
    {166, 256}, {192, 256}, {210, 256}, {242, 256}, {242, 256}, {210, 256}, {192, 256}, {166, 256},
    {166, 256}, {192, 256}, {220, 256}, {268, 256}, {268, 256}, {220, 256}, {192, 256}, {166, 256},
    {166, 256}, {192, 256}, {220, 256}, {242, 256}, {242, 256}, {220, 256}, {192, 256}, {166, 256},
    {166, 256}, {192, 256}, {210, 256}, {216, 256}, {216, 256}, {210, 256}, {192, 256}, {166, 256},
    {166, 256}, {192, 256}, {204, 256}, {216, 256}, {216, 256}, {204, 256}, {192, 256}, {166, 256},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
  },
  {
    // Knight
    {704, 730}, {730, 756}, {756, 781}, {768, 794}, {768, 794}, {756, 781}, {730, 756}, {704, 730},
    {743, 756}, {768, 781}, {794, 807}, {807, 820}, {807, 820}, {794, 807}, {768, 781}, {743, 756},
    {781, 781}, {807, 807}, {832, 832}, {844, 844}, {844, 844}, {832, 832}, {807, 807}, {781, 781},
    {807, 794}, {832, 820}, {857, 844}, {870, 857}, {870, 857}, {857, 844}, {832, 820}, {807, 794},
    {820, 794}, {844, 820}, {870, 844}, {883, 857}, {883, 857}, {870, 844}, {844, 820}, {820, 794},
    {820, 781}, {844, 807}, {870, 832}, {883, 844}, {883, 844}, {870, 832}, {844, 807}, {820, 781},
    {781, 756}, {807, 781}, {832, 807}, {844, 820}, {844, 820}, {832, 807}, {807, 781}, {781, 756},
    {650, 730}, {768, 756}, {794, 781}, {807, 794}, {807, 794}, {794, 781}, {768, 756}, {650, 730},
  },
  {
    // Bishop
    {786, 786}, {786, 802}, {792, 809}, {797, 817}, {797, 817}, {792, 809}, {786, 802}, {786, 786},
    {812, 802}, {832, 817}, {827, 825}, {832, 832}, {832, 832}, {827, 825}, {832, 817}, {812, 802},
    {817, 809}, {827, 825}, {842, 832}, {837, 839}, {837, 839}, {842, 832}, {827, 825}, {817, 809},
    {822, 817}, {832, 832}, {837, 839}, {852, 847}, {852, 847}, {837, 839}, {832, 832}, {822, 817},
    {822, 817}, {832, 832}, {837, 839}, {852, 847}, {852, 847}, {837, 839}, {832, 832}, {822, 817},
    {817, 809}, {827, 825}, {842, 832}, {837, 839}, {837, 839}, {842, 832}, {827, 825}, {817, 809},
    {812, 802}, {832, 817}, {827, 825}, {832, 832}, {832, 832}, {827, 825}, {832, 817}, {812, 802},
    {812, 786}, {812, 802}, {817, 809}, {822, 817}, {822, 817}, {817, 809}, {812, 802}, {812, 786},
  },
  {
    // Rook
    {1267, 1282}, {1275, 1282}, {1282, 1282}, {1289, 1282}, {1289, 1282}, {1282, 1282},
    {1275, 1282}, {1267, 1282}, {1267, 1282}, {1275, 1282}, {1282, 1282}, {1289, 1282},
    {1289, 1282}, {1282, 1282}, {1275, 1282}, {1267, 1282}, {1267, 1282}, {1275, 1282},
    {1282, 1282}, {1289, 1282}, {1289, 1282}, {1282, 1282}, {1275, 1282}, {1267, 1282},
    {1267, 1282}, {1275, 1282}, {1282, 1282}, {1289, 1282}, {1289, 1282}, {1282, 1282},
    {1275, 1282}, {1267, 1282}, {1267, 1282}, {1275, 1282}, {1282, 1282}, {1289, 1282},
    {1289, 1282}, {1282, 1282}, {1275, 1282}, {1267, 1282}, {1267, 1282}, {1275, 1282},
    {1282, 1282}, {1289, 1282}, {1289, 1282}, {1282, 1282}, {1275, 1282}, {1267, 1282},
    {1267, 1282}, {1275, 1282}, {1282, 1282}, {1289, 1282}, {1289, 1282}, {1282, 1282},
    {1275, 1282}, {1267, 1282}, {1267, 1282}, {1275, 1282}, {1282, 1282}, {1289, 1282},
    {1289, 1282}, {1282, 1282}, {1275, 1282}, {1267, 1282},
  },
  {
    // Queen
    {2560, 2499}, {2560, 2520}, {2560, 2530}, {2560, 2540}, {2560, 2540}, {2560, 2530},
    {2560, 2520}, {2560, 2499}, {2560, 2520}, {2560, 2540}, {2560, 2550}, {2560, 2560},
    {2560, 2560}, {2560, 2550}, {2560, 2540}, {2560, 2520}, {2560, 2530}, {2560, 2550},
    {2560, 2560}, {2560, 2570}, {2560, 2570}, {2560, 2560}, {2560, 2550}, {2560, 2530},
    {2560, 2540}, {2560, 2560}, {2560, 2570}, {2560, 2580}, {2560, 2580}, {2560, 2570},
    {2560, 2560}, {2560, 2540}, {2560, 2540}, {2560, 2560}, {2560, 2570}, {2560, 2580},
    {2560, 2580}, {2560, 2570}, {2560, 2560}, {2560, 2540}, {2560, 2530}, {2560, 2550},
    {2560, 2560}, {2560, 2570}, {2560, 2570}, {2560, 2560}, {2560, 2550}, {2560, 2530},
    {2560, 2520}, {2560, 2540}, {2560, 2550}, {2560, 2560}, {2560, 2560}, {2560, 2550},
    {2560, 2540}, {2560, 2520}, {2560, 2499}, {2560, 2520}, {2560, 2530}, {2560, 2540},
    {2560, 2540}, {2560, 2530}, {2560, 2520}, {2560, 2499},
  },
  {
    // King
    {302, 16},  {328, 78},  {276, 108}, {225, 139}, {225, 139}, {276, 108}, {328, 78},  {302, 16},
    {276, 78},  {302, 139}, {251, 170}, {200, 200}, {200, 200}, {251, 170}, {302, 139}, {276, 78},
    {225, 108}, {251, 170}, {200, 200}, {149, 230}, {149, 230}, {200, 200}, {251, 170}, {225, 108},
    {200, 139}, {225, 200}, {175, 230}, {124, 261}, {124, 261}, {175, 230}, {225, 200}, {200, 139},
    {175, 139}, {200, 200}, {149, 230}, {98, 261},  {98, 261},  {149, 230}, {200, 200}, {175, 139},
    {149, 108}, {175, 170}, {124, 200}, {72, 230},  {72, 230},  {124, 200}, {175, 170}, {149, 108},
    {124, 78},  {149, 139}, {98, 170},  {47, 200},  {47, 200},  {98, 170},  {149, 139}, {124, 78},
    {98, 16},   {124, 78},  {72, 108},  {21, 139},  {21, 139},  {72, 108},  {124, 78},  {98, 16},
  },
};

constexpr std::pair<Value, Value> KnightMobilityMult = {6, 6};
constexpr std::pair<Value, Value> BishopMobilityMult = {2, 3};
constexpr std::pair<Value, Value> RookMobilityMult   = {3, 6};
constexpr std::pair<Value, Value> QueenMobilityMult  = {2, 7};

constexpr Bitboard PassedPawnMask[Color::COLOR_NB][Square::SQ_NB] = {
  // White
  {
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    847736400445440ULL,
    1978051601039360ULL,
    3956103202078720ULL,
    7912206404157440ULL,
    15824412808314880ULL,
    31648825616629760ULL,
    27127564814254080ULL,
    54255129628508160ULL,
    847736400248832ULL,
    1978051600580608ULL,
    3956103201161216ULL,
    7912206402322432ULL,
    15824412804644864ULL,
    31648825609289728ULL,
    27127564807962624ULL,
    54255129615925248ULL,
    847736349917184ULL,
    1978051483140096ULL,
    3956102966280192ULL,
    7912205932560384ULL,
    15824411865120768ULL,
    31648823730241536ULL,
    27127563197349888ULL,
    54255126394699776ULL,
    847723465015296ULL,
    1978021418369024ULL,
    3956042836738048ULL,
    7912085673476096ULL,
    15824171346952192ULL,
    31648342693904384ULL,
    27127150880489472ULL,
    54254301760978944ULL,
    844424930131968ULL,
    1970324836974592ULL,
    3940649673949184ULL,
    7881299347898368ULL,
    15762598695796736ULL,
    31525197391593472ULL,
    27021597764222976ULL,
    54043195528445952ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
  },
  // Black
  {
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    768ULL,
    1792ULL,
    3584ULL,
    7168ULL,
    14336ULL,
    28672ULL,
    24576ULL,
    49152ULL,
    197376ULL,
    460544ULL,
    921088ULL,
    1842176ULL,
    3684352ULL,
    7368704ULL,
    6316032ULL,
    12632064ULL,
    50529024ULL,
    117901056ULL,
    235802112ULL,
    471604224ULL,
    943208448ULL,
    1886416896ULL,
    1616928768ULL,
    3233857536ULL,
    12935430912ULL,
    30182672128ULL,
    60365344256ULL,
    120730688512ULL,
    241461377024ULL,
    482922754048ULL,
    413933789184ULL,
    827867578368ULL,
    3311470314240ULL,
    7726764066560ULL,
    15453528133120ULL,
    30907056266240ULL,
    61814112532480ULL,
    123628225064960ULL,
    105967050055680ULL,
    211934100111360ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
    0ULL,
  }};

Value evaluate(const Position& pos) {
    static constexpr int PieceTypeValues[] = {1, 3, 3, 5, 9};
    Color                us                = pos.side_to_move();
    Bitboard             white_pieces      = pos.pieces(Color::WHITE);
    Bitboard             black_pieces      = pos.pieces(Color::BLACK);
    Value                mid_game_score    = 0;
    Value                end_game_score    = 0;
    int                  phase             = 0;
    int                  coeff             = 1;
    for (Color c : {Color::WHITE, Color::BLACK}) {
        for (Square sq : pos.pieces(c)) {
            PieceType pt = type(pos.piece_on(sq));
            mid_game_score += coeff * PieceSquareTable[pt][sq.to_int()].first;
            end_game_score += coeff * PieceSquareTable[pt][sq.to_int()].second;
            if (pt != PieceType::KING) {
                phase += PieceTypeValues[pt];
            }
            // Piece Mobility Bonus
            if (pt == PieceType::KNIGHT) {
                Bitboard to =
                  knight_attacks[sq.to_int()] - (us == Color::WHITE ? white_pieces : black_pieces);
                int counts = to.count();
                mid_game_score += coeff * KnightMobilityMult.first * counts;
                end_game_score += coeff * KnightMobilityMult.second * counts;
            }
            if (pt == PieceType::BISHOP) {
                Bitboard bishop_attacks = bishop_magics[sq.to_int()](white_pieces | black_pieces);
                bishop_attacks -= (us == Color::WHITE ? white_pieces : black_pieces);
                int counts = bishop_attacks.count();
                mid_game_score += coeff * BishopMobilityMult.first * counts;
                end_game_score += coeff * BishopMobilityMult.second * counts;
            }
            if (pt == PieceType::ROOK) {
                Bitboard rook_attacks = rook_magics[sq.to_int()](white_pieces | black_pieces);
                rook_attacks -= (us == Color::WHITE ? white_pieces : black_pieces);
                int counts = rook_attacks.count();
                mid_game_score += coeff * RookMobilityMult.first * counts;
                end_game_score += coeff * RookMobilityMult.second * counts;
            }
            if (pt == PieceType::QUEEN) {
                Bitboard rook_attacks   = rook_magics[sq.to_int()](white_pieces | black_pieces);
                Bitboard bishop_attacks = bishop_magics[sq.to_int()](white_pieces | black_pieces);
                Bitboard queen_attacks  = rook_attacks | bishop_attacks;
                queen_attacks -= (us == Color::WHITE ? white_pieces : black_pieces);
                int counts = queen_attacks.count();
                mid_game_score += coeff * QueenMobilityMult.first * counts;
                end_game_score += coeff * QueenMobilityMult.second * counts;
            }
        }
        // Passed pawn bonus.
        const Bitboard& opponent_pawns = pos.pieces(other_color(c), PieceType::PAWN);
        for (Square sq : pos.pieces(c, PieceType::PAWN)) {
            Bitboard visible_pawns = PassedPawnMask[c][sq.to_int()] & opponent_pawns;
            if (visible_pawns.empty()) {
                int promotion_rank = (c == Color::WHITE ? 8 : 1);
                int bonus = 200 - 25 * std::abs(static_cast<int>(sq.rank()) - promotion_rank);
                mid_game_score += coeff * bonus;
                end_game_score += coeff * bonus * 3 / 2;
            }
        }
        coeff *= -1;
    }
    Value score = mid_game_score * phase + end_game_score * (78 - phase);
    score /= 195;
    return us == Color::WHITE ? score : -score;
}

} // namespace sonic