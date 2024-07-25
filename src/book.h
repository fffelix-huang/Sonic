#pragma once

#include <cstdint>
#include <string>

#include "chess/all.h"

namespace sonic {

class Book {
public:
    Book() {}

    Book(const std::string& file) {
        open(file);
    }

    struct Entry {
        std::uint64_t key;
        std::uint16_t move;
        std::uint16_t count;
        std::uint16_t n;
        std::uint16_t sum;
    };

    constexpr bool is_open() const { return book != NULL; }

    void open(const std::string& file);
    void close();

    // Select a random book move.
    Move book_move(const Position& pos) const;

private:
    int find_key(std::uint64_t key) const;
    Entry read_entry(int pos) const;

    FILE* book = NULL;
    int book_size = 0;
};

} // namespace sonic