#include <algorithm>     // For standard algorithms like std::sort, std::find, etc.
#include <iostream>      // For input/output operations (std::cout, std::cin)
#include <string>        // For string manipulation (std::string)
#include <vector>        // For dynamic arrays (std::vector)

#include "bench/perft.h"     	// For performance testing using the Perft algorithm
#include "bench/benchmark.h" 	// For general benchmarking and performance measurement
#include "chess/all.h"       	// For all chess-related functions and data structures
#include "utils/bits.h"      	// For bit manipulation utilities, such as bitwise operations and masks
#include "utils/random.h"    	// For random number generation functions
#include "utils/small_vector.h" // For a small, fixed-size vector implementation optimized for performance
#include "utils/strings.h" 		// For string manipulation utilities, such as string comparison and formatting
#include "uci.h" 				// For implementing the Universal Chess Interface (UCI) protocol for communication with chess GUIs and engines
#include "search.h" 			// For search algorithms used to find the best move in a chess position

int main(int argc, char* argv[]) {
    using namespace std; // Using namespace std for convenience
    using namespace sonic; // Using namespace sonic for Sonic-specific classes and functions

    // Initialize attack tables for efficient move generation
    init_attacks();

    // Check if the "bench" command-line argument is provided
    if (argc > 1 && std::string(argv[1]) == "bench") {
        // Run benchmarking tests
        run_bench();
        return 0; // Exit successfully
    }

    // Start the UCI loop for interacting with a chess GUI or engine
    uci_loop();

    return 0; // Exit successfully
}