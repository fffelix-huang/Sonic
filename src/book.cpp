#include "book.h"

#include <cassert>
#include <cstdint>
#include <string>

#include "chess/all.h"
#include "utils/random.h"

namespace sonic {

void Book::open(const std::string& file) {
    book = fopen(file.c_str(), "rb+");
    assert(book != NULL);
    fseek(book, 0, SEEK_END);
    book_size = ftell(book) / 16;
}

void Book::close() {
    assert(fclose(book) != EOF);
    book_size = 0;
}

// Select a random book move.
Move Book::book_move(const Position& pos) const {
    if(!is_open()) {
        return NO_MOVE;
    }
    // Convert polyglot move format to ours.
    auto convert_move = [&](int move) -> Move {
        int to_file = move & 7;
        int to_rank = (move >> 3) & 7;
        int from_file = (move >> 6) & 7;
        int from_rank = (move >> 9) & 7;
        int promotion_piece = (move >> 12) & 7;
        Square from = Square(File(from_file), Rank(from_rank));
        Square to = Square(File(to_file), Rank(to_rank));
        Move::Promotion promotion = Move::Promotion::None;
        if(promotion_piece != 0) {
            // Our piece order is reversed.
            promotion = Move::Promotion(5 - promotion_piece);
        }
        return Move(from, to, promotion);
    };
    Move best_move = NO_MOVE;
    int best_score = 0;
    for(int i = find_key(pos.hashkey()); i < book_size; i++) {
        Book::Entry entry = read_entry(i);
        if(entry.key != pos.hashkey()) {
            break;
        }
        int score = entry.count;
        best_score += score;
        if(rng() % best_score < score) {
            best_move = convert_move(entry.move);
        }
    }
    return best_move;
}

// Binary search the file to find the first occurance of the key.
int Book::find_key(std::uint64_t key) const {
    int l = 0, r = book_size - 1;
    while(l < r) {
        int mid = (l + r) / 2;
        Book::Entry entry = read_entry(mid);
        if(key <= entry.key) {
            r = mid;
        } else {
            l = mid + 1;
        }
    }
    Book::Entry entry = read_entry(l);
    return (entry.key == key ? l : book_size);
}

Book::Entry Book::read_entry(int pos) const {
    auto read_int = [&](int bytes) -> std::uint64_t {
        std::uint64_t result;
        for(int i = 0; i < bytes; i++) {
            int byte = fgetc(book);
            assert(byte != EOF);
            result = (result << 8) | byte;
        }
        return result;
    };
    assert(fseek(book, pos * 16, SEEK_SET) == 0);
    Book::Entry entry;
    entry.key = read_int(8);
    entry.move = read_int(2);
    entry.count = read_int(2);
    entry.n = read_int(2);
    entry.sum = read_int(2);
    return entry;
}

} // namespace sonic