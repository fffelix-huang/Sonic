#include "attacks.h"

#include "bitboard.h"
#include "color.h"

namespace sonic {

Bitboard rook_rays[Square::SQ_NB];
Magic<4096> rook_magics[Square::SQ_NB];
Bitboard bishop_rays[Square::SQ_NB];
Magic<512> bishop_magics[Square::SQ_NB];

void init_rook_attacks() {
    for(int file = 0; file < 8; file++) {
        for(int rank = 0; rank < 8; rank++) {
            Bitboard rays_bb(0);

            // North
            for(int new_rank = rank + 1; new_rank < 7; new_rank++) {
                rays_bb += Square(File(file), Rank(new_rank));
            }

            // South
            for(int new_rank = rank - 1; new_rank > 0; new_rank--) {
                rays_bb += Square(File(file), Rank(new_rank));
            }

            // West
            for(int new_file = file - 1; new_file > 0; new_file--) {
                rays_bb += Square(File(new_file), Rank(rank));
            }

            // East
            for(int new_file = file + 1; new_file < 7; new_file++) {
                rays_bb += Square(File(new_file), Rank(rank));
            }

            Square sq = Square(File(file), Rank(rank));
            rook_rays[sq.to_int()] = rays_bb;

            // Iterate over all submask of rays to fill magic table.
            for(std::uint64_t blockers = rays_bb.to_int(); ; blockers = (blockers - 1) & rays_bb.to_int()) {
                const Bitboard& blockers_bb(blockers);
                Bitboard attacks_bb = 0;

                // North
                for(int new_rank = rank + 1; Square::is_valid(new_rank, file); new_rank++) {
                    Square cur = Square(File(file), Rank(new_rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // South
                for(int new_rank = rank - 1; Square::is_valid(new_rank, file); new_rank--) {
                    Square cur = Square(File(file), Rank(new_rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // West
                for(int new_file = file - 1; Square::is_valid(rank, new_file); new_file--) {
                    Square cur = Square(File(new_file), Rank(rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // East
                for(int new_file = file + 1; Square::is_valid(rank, new_file); new_file++) {
                    Square cur = Square(File(new_file), Rank(rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // Update magic table.
                std::uint64_t key = (blockers * rook_multiplies[sq.to_int()]) >> rook_shifts[sq.to_int()];
                rook_magics[sq.to_int()].ray_mask = rays_bb;
                rook_magics[sq.to_int()].magic = rook_multiplies[sq.to_int()];
                rook_magics[sq.to_int()].shift = rook_shifts[sq.to_int()];
                rook_magics[sq.to_int()].attacks[key] = attacks_bb;

                if(blockers == 0) {
                    break;
                }
            }
        }
    }
}

void init_bishop_attacks() {
    for(int file = 0; file < 8; file++) {
        for(int rank = 0; rank < 8; rank++) {
            Bitboard rays_bb(0);

            // North West
            for(int new_file = file - 1, new_rank = rank + 1; new_file > 0 && new_rank < 7; new_file--, new_rank++) {
                Square cur = Square(File(new_file), Rank(new_rank));
                rays_bb += cur;
            }

            // North East
            for(int new_file = file + 1, new_rank = rank + 1; new_file < 7 && new_rank < 7; new_file++, new_rank++) {
                Square cur = Square(File(new_file), Rank(new_rank));
                rays_bb += cur;
            }

            // South West
            for(int new_file = file - 1, new_rank = rank - 1; new_file > 0 && new_rank > 0; new_file--, new_rank--) {
                Square cur = Square(File(new_file), Rank(new_rank));
                rays_bb += cur;
            }

            // South East
            for(int new_file = file + 1, new_rank = rank - 1; new_file < 7 && new_rank > 0; new_file++, new_rank--) {
                Square cur = Square(File(new_file), Rank(new_rank));
                rays_bb += cur;
            }

            Square sq = Square(File(file), Rank(rank));
            bishop_rays[sq.to_int()] = rays_bb;

            // Iterate over all submask of rays to fill magic table.
            for(std::uint64_t blockers = rays_bb.to_int(); ; blockers = (blockers - 1) & rays_bb.to_int()) {
                const Bitboard& blockers_bb(blockers);
                Bitboard attacks_bb = 0;

                // North West
                for(int new_file = file - 1, new_rank = rank + 1; Square::is_valid(new_rank, new_file); new_file--, new_rank++) {
                    Square cur = Square(File(new_file), Rank(new_rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // North East
                for(int new_file = file + 1, new_rank = rank + 1; Square::is_valid(new_rank, new_file); new_file++, new_rank++) {
                    Square cur = Square(File(new_file), Rank(new_rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // South West
                for(int new_file = file - 1, new_rank = rank - 1; Square::is_valid(new_rank, new_file); new_file--, new_rank--) {
                    Square cur = Square(File(new_file), Rank(new_rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // South East
                for(int new_file = file + 1, new_rank = rank - 1; Square::is_valid(new_rank, new_file); new_file++, new_rank--) {
                    Square cur = Square(File(new_file), Rank(new_rank));
                    attacks_bb += cur;
                    if(blockers_bb.get(cur)) {
                        break;
                    }
                }

                // Update magic table.
                std::uint64_t key = (blockers * bishop_multiplies[sq.to_int()]) >> bishop_shifts[sq.to_int()];
                bishop_magics[sq.to_int()].ray_mask = rays_bb;
                bishop_magics[sq.to_int()].magic = bishop_multiplies[sq.to_int()];
                bishop_magics[sq.to_int()].shift = bishop_shifts[sq.to_int()];
                bishop_magics[sq.to_int()].attacks[key] = attacks_bb;

                if(blockers == 0) {
                    break;
                }
            }
        }
    }
}

void init_attacks() {
    init_rook_attacks();
    init_bishop_attacks();
}

} // namespace sonic