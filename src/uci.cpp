#include "uci.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <limits>
#include <vector>
#include <string>
#include <thread>

#include "bench/benchmark.h"
#include "bench/perft.h"
#include "chess/all.h"
#include "utils/strings.h"
#include "utils/timer.h"
#include "search.h"
#include "ucioption.h"
#include "version.h"
#include <mutex> // Added for thread safety

namespace sonic {

// Mutex for thread safety when accessing shared resources
std::mutex mtx;

OptionsMap init_options_map() {
    OptionsMap options;
    options.add_option("Book", "string", "<none>");
    options.add_option("Hash", "spin", 16, 1, 1024);
    options.add_option("Threads", "spin", 1, 1, 1); // Doesn't support multi-threading currently.
    options.add_option("ClearHash", "button", [&]() -> void { TT.clear(); });
    return options;
}

OptionsMap options = init_options_map();

void uci_loop() {
    std::cout << "Sonic Chess Engine " << version_to_string() << " by Ting-Hsuan Huang" << std::endl;
    Position pos;
    SearchInfo search_info;
    std::string cmd;
    std::thread th;

    while (std::getline(std::cin, cmd)) {
        if (cmd.empty()) {
            continue;
        }

        std::vector<std::string> tokens = split_string(cmd, ' ');

        // Handle "setoption" command
        if (tokens[0] == "setoption") {
            assert(tokens[1] == "name");
            if (tokens.size() == 3) {
                // Button type option
                options.button(tokens[2]);
            } else if (tokens.size() == 5) {
                assert(tokens[3] == "value");
                options.set(tokens[2], tokens[4]);
                if (tokens[2] == "Hash") {
                    TT.resize(int(options["Hash"]));
                }
            } else {
                std::cout << "Invalid setoption command format." << std::endl;
            }

        // Handle "quit" command
        } else if (tokens[0] == "quit") {
            std::exit(0);

        // Handle "uci" command
        } else if (tokens[0] == "uci") {
            std::cout << "id name Sonic " << version_to_string() << std::endl;
            std::cout << "id author Ting-Hsuan Huang" << std::endl;
            std::cout << options << std::endl;
            std::cout << "uciok" << std::endl;

        // Handle "isready" command
        } else if (tokens[0] == "isready") {
            std::cout << "readyok" << std::endl;

        // Handle "ucinewgame" command
        } else if (tokens[0] == "ucinewgame") {
            if (th.joinable()) {
                th.join();
            }
            pos.set(INITIAL_FEN);
            TT.clear();

        // Handle "bench" command
        } else if (tokens[0] == "bench") {
            run_bench();

        // Handle "perft" command
        } else if (tokens[0] == "perft") {
            bench_perft();

        // Handle "position" command
        } else if (tokens[0] == "position") {
            parse_position(pos, search_info, tokens);

        // Handle "go" command
        } else if (tokens[0] == "go") {
            if (th.joinable()) {
                th.join();
            }
            parse_go(pos, search_info, tokens);
            
            // Lock shared resources before starting search thread
            mtx.lock();
            th = std::thread([&]() {
                search(pos, search_info);
                mtx.unlock(); // Unlock resources after the search is done
            });

        // Handle "stop" command
        } else if (tokens[0] == "stop") {
            search_info.stop = true;
            if (th.joinable()) {
                th.join();
            }

        // Handle "d" command to display the board
        } else if (tokens[0] == "d") {
            std::cout << pos.to_string() << std::endl;

        // Handle "tune" command
        } else if (tokens[0] == "tune") {
            options.print_tune_params();

        // Handle unknown commands
        } else {
            std::cout << "Unknown Command: " << cmd << std::endl;
            std::cout << "Available commands: setoption, quit, uci, isready, ucinewgame, bench, perft, position, go, stop, d, tune." << std::endl;
        }
    }
}

// Function to parse the "position" command
void parse_position(Position& pos, SearchInfo& search_info, const std::vector<std::string>& tokens) {
    int moves_start = -1;
    if (tokens[1] == "fen") {
        std::string fen;
        for (int i = 0; i < 6; i++) {
            fen += tokens[2 + i];
            if (i != 5) {
                fen += " ";
            }
        }
        pos.set(fen);
        moves_start = 8;
    } else if (tokens[1] == "startpos") {
        pos.set(INITIAL_FEN);
        moves_start = 2;
    }

    if (static_cast<int>(tokens.size()) <= moves_start || tokens[moves_start] != "moves") {
        return;
    }

    for (size_t i = moves_start + 1; i < tokens.size(); i++) {
        Square from = Square(tokens[i][1] - '1', tokens[i][0] - 'a');
        Square to = Square(tokens[i][3] - '1', tokens[i][2] - 'a');
        Move::Promotion promotion = Move::Promotion::None;

        if (tokens[i].size() == 5) {
            switch (tokens[i][4]) {
                case 'q': promotion = Move::Promotion::Queen; break;
                case 'n': promotion = Move::Promotion::Knight; break;
                case 'r': promotion = Move::Promotion::Rook; break;
                case 'b': promotion = Move::Promotion::Bishop; break;
                default: break;
            }
        }

        Move move(from, to, promotion);
        UndoInfo info;
        assert(pos.make_move(move, info));
    }
}

// Function to parse the "go" command
void parse_go(Position& pos, SearchInfo& search_info, const std::vector<std::string>& params) {
    const Color& us = pos.side_to_move();
    search_info = SearchInfo();
    int time = -1, increment = 0;

    for (size_t i = 1; i < params.size(); i++) {
        if (params[i] == "movetime") {
            i++;
            time = stoi(params[i]);
        } else if (params[i] == "wtime" && us == Color::WHITE) {
            i++;
            time = stoi(params[i]);
        } else if (params[i] == "btime" && us == Color::BLACK) {
            i++;
            time = stoi(params[i]);
        } else if (params[i] == "winc" && us == Color::WHITE) {
            i++;
            increment = stoi(params[i]);
        } else if (params[i] == "binc" && us == Color::BLACK) {
            i++;
            increment = stoi(params[i]);
        } else if (params[i] == "nodes") {
            i++;
            search_info.max_nodes = stoi(params[i]);
        } else if (params[i] == "depth") {
            i++;
            search_info.max_depth = std::min(stoi(params[i]), MAX_DEPTH);
        }
    }

    // If no time control is specified, use infinite time
    if (time == -1) {
        time = std::numeric_limits<int>::max() / 2;
    }

    search_info.nodes = 0;
    search_info.start_time = current_time();
    search_info.max_time = time / 15 + increment / 2;
    search_info.stop = false;
}

} // namespace sonic
