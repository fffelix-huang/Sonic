// Header files
#include "book.h" // Include the header file for the Book class
#include <cassert> // For assertion macros (assert)
#include <cstdint> // For fixed-width integer types (uint64_t)
#include <string> // For string manipulation (std::string)
#include "chess/all.h" // Include all chess-related functions and data structures
#include "utils/random.h" // Include random number generation utilities

// Declare the sonic namespace
namespace sonic {

// Function to open a book file
void Book::open(const std::string& file) {
    // Open the book file in read/write mode
    book = fopen(file.c_str(), "rb+");

    // Check if the file was opened successfully
    if (book == NULL) {
        return; // Return if the file could not be opened
    }

    // Seek to the end of the file
    fseek(book, 0, SEEK_END);

    // Calculate the book size in entries (assuming 16 bytes per entry)
    book_size = ftell(book) / 16;
}

// Function to close the book file
void Book::close() {
    // Close the book file and check for errors
    assert(fclose(book) != EOF);

    // Reset the book size to 0
    book_size = 0;
}

// Function to select a random book move
Move Book::book_move(const Position& pos) const {
    // Check if the book is open
    if (!is_open()) {
        return MOVE_NONE; // Return MOVE_NONE if the book is not open
    }

    // Convert polyglot move format to our format
    auto convert_move = [&](int move) -> Move {
        // Extract the move components from the polyglot format
        int to_file = move & 7;
        int to_rank = (move >> 3) & 7;
        int from_file = (move >> 6) & 7;
        int from_rank = (move >> 9) & 7;
        int promotion_piece = (move >> 12) & 7;

        // Create a Move object from the extracted components
        Square from = Square(File(from_file), Rank(from_rank));
        Square to = Square(File(to_file), Rank(to_rank));
        Move::Promotion promotion = Move::Promotion::None;
        if (promotion_piece != 0) {
            // Our piece order is reversed
            promotion = Move::Promotion(5 - promotion_piece);
        }
        return Move(from, to, promotion);
    };

    // Generate all legal moves for the given position
    MoveList movelist;
    generate_moves<GenType::ALL>(pos, movelist);

    // Initialize variables for the best move and score
    Move best_move = MOVE_NONE;
    int best_score = 0;

    // Iterate through the book entries
    for (int i = find_key(pos.hashkey()); i < book_size; i++) {
        // Read the book entry
        Book::Entry entry = read_entry(i);

        // Check if the entry's key matches the position's hashkey
        if (entry.key != pos.hashkey()) {
            break; // Break the loop if the keys don't match
        }

        // Convert the polyglot move to our format
        Move move = convert_move(entry.move);

        // Check if the move is legal for the given position
        if (!movelist.contains(move)) {
            continue; // Skip the move if it's not legal
        }

        // Update the best move and score if necessary
        int score = entry.count;
        best_score += score;
        if (int(rng() % best_score) < score) {
            best_move = move;
        }
    }

    // Return the best book move
    return best_move;
}

// Function to perform binary search for a key in the book
int Book::find_key(std::uint64_t key) const {
    // Initialize left and right indices
    int l = 0, r = book_size - 1;

    // Perform binary search
    while (l < r) {
        int mid = (l + r) / 2;
        Book::Entry entry = read_entry(mid);
        if (key <= entry.key) {
            r = mid;
        } else {
            l = mid + 1;
        }
    }

    // Return the index of the entry or book_size if not found
    Book::Entry entry = read_entry(l);
    return (entry.key == key ? l : book_size);
}

// Function to read a book entry from the file
Book::Entry Book::read_entry(int pos) const {
    // Function to read an integer from the file
    auto read_int = [&](int bytes) -> std::uint64_t {
        std::uint64_t result = 0;
        for (int i = 0; i < bytes; i++) {
            int byte = fgetc(book);
            assert(byte != EOF);
            result = (result << 8) | byte;
        }
        return result;
    };

    // Seek to the specified position in the file
    assert(fseek(book, pos * 16, SEEK_SET) == 0);

    // Read the entry from the file
    Book::Entry entry;
    entry.key = read_int(8);
    entry.move = read_int(2);
    entry.count = read_int(2);
    entry.n = read_int(2);
    entry.sum = read_int(2);

    // Return the read entry
    return entry;
}

} // namespace sonic