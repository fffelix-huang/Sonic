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
#include "book.h"
#include "search.h"

namespace sonic {

void uci_loop() {
    std::cout << "Sonic Chess Engine, written by Ting-Hsuan Huang" << std::endl;
    Position pos;
    SearchInfo search_info;
    TT.resize(16);
    Book book;
    std::string cmd;
    std::thread th;
    while(std::getline(std::cin, cmd)) {
        if(cmd == "") {
            continue;
        }
        std::vector<std::string> tokens = split_string(cmd, ' ');
        if(tokens[0] == "setoption") {
            assert(tokens[1] == "name");
            if(tokens[2] == "book") {
                assert(tokens[3] == "value");
                book.open(tokens[4]);
            } else if(tokens[2] == "hash") {
                assert(tokens[3] == "value");
                int mb = stoi(tokens[4]);
                mb = std::clamp(mb, 1, 1024);
                TT.resize(mb);
            } else if(tokens[2] == "clearhash") {
                TT.clear();
            } else {
                std::cout << "Unknown option." << std::endl;
            }
        } else if(tokens[0] == "quit") {
            std::exit(0);
        } else if(tokens[0] == "uci") {
            std::cout << "id name Sonic" << std::endl;
            std::cout << "id author Ting-Hsuan Huang" << std::endl;
            std::cout << "option name book type string default <none>" << std::endl;
            std::cout << "option name hash type spin default 16 min 1 max 1024" << std::endl;
            std::cout << "option name clearhash type button" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if(tokens[0] == "isready") {
            std::cout << "readyok" << std::endl;
        } else if(tokens[0] == "ucinewgame") {
            if(th.joinable()) {
                th.join();
            }
            pos.set(INITIAL_FEN);
            TT.clear();
        } else if(tokens[0] == "bench") {
            run_bench();
        } else if(tokens[0] == "perft") {
            bench_perft();
        } else if(tokens[0] == "position") {
            parse_position(pos, search_info, tokens);
        } else if(tokens[0] == "go") {
            if(th.joinable()) {
                th.join();
            }
            parse_go(pos, search_info, tokens);
            th = std::thread(search, std::ref(pos), std::ref(search_info), book);
        } else if(tokens[0] == "stop") {
            search_info.stop = true;
            if(th.joinable()) {
                th.join();
            }
        } else if(tokens[0] == "d") {
            std::cout << pos.to_string() << std::endl;
        } else {
            std::cout << "Unknown Command: " << cmd << std::endl;
        }
    }
}

void parse_position(Position& pos, SearchInfo& search_info, const std::vector<std::string>& tokens) {
    int moves_start = -1;
    if(tokens[1] == "fen") {
        std::string fen;
        for(int i = 0; i < 6; i++) {
            fen += tokens[2 + i];
            if(i != 5) {
                fen += " ";
            }
        }
        pos.set(fen);
        moves_start = 8;
    }
    if(tokens[1] == "startpos") {
        pos.set(INITIAL_FEN);
        moves_start = 2;
    }
    if(tokens.size() <= moves_start) {
        return;
    }
    if(tokens[moves_start] != "moves") {
        return;
    }
    for(int i = moves_start + 1; i < tokens.size(); i++) {
        Square from = Square(tokens[i][1] - '1', tokens[i][0] - 'a');
        Square to = Square(tokens[i][3] - '1', tokens[i][2] - 'a');
        Move::Promotion promotion = Move::Promotion::None;
        if(tokens[i].size() == 5) {
            switch(tokens[i][4]) {
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

void parse_go(Position& pos, SearchInfo& search_info, const std::vector<std::string>& params) {
    const Color& us = pos.side_to_move();
    search_info = SearchInfo();
    int time = -1, increment = 0;
    for(int i = 1; i < params.size(); i++) {
        if(params[i] == "wtime" && us == Color::WHITE) {
            i++;
            time = stoi(params[i]);
        } else if(params[i] == "btime" && us == Color::BLACK) {
            i++;
            time = stoi(params[i]);
        } else if(params[i] == "winc" && us == Color::WHITE) {
            i++;
            increment = stoi(params[i]);
        } else if(params[i] == "binc" && us == Color::BLACK) {
            i++;
            increment = stoi(params[i]);
        } else if(params[i] == "nodes") {
            i++;
            search_info.max_nodes = stoi(params[i]);
        } else if(params[i] == "depth") {
            i++;
            search_info.max_depth = std::min(stoi(params[i]), MAX_DEPTH);
        }
    }
    if(time == -1) {
        // go infinite
        time = std::numeric_limits<int>::max() / 2;
    }
    search_info.nodes = 0;
    search_info.start_time = current_time();
    search_info.max_time = time / 15 + increment / 2;
    search_info.stop = false;
}

} // namespace sonic