#include "uci.h"

#include <cassert>
#include <iostream>
#include <istream>
#include <limits>
#include <vector>
#include <string>

#include "bench/benchmark.h"
#include "chess/all.h"
#include "utils/strings.h"
#include "utils/timer.h"
#include "search.h"

namespace sonic {

void uci_loop(Position& pos, SearchInfo& search_info) {
    std::cout << "Sonic Chess Engine, written by Ting-Hsuan Huang" << std::endl;
    std::string cmd;
    while(std::getline(std::cin, cmd)) {
        std::vector<std::string> tokens = split_string(cmd, ' ');
        if(tokens[0] == "quit") {
            std::exit(0);
        }
        if(tokens[0] == "uci") {
            std::cout << "id name Sonic" << std::endl;
            std::cout << "id author Ting-Hsuan Huang" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        if(tokens[0] == "isready") {
            std::cout << "readyok" << std::endl;
        }
        if(tokens[0] == "ucinewgame") {
            pos.set(INITIAL_FEN);
        }
        if(tokens[0] == "bench") {
            run_bench();
        }
        if(tokens[0] == "position") {
            parse_position(pos, search_info, tokens);
        }
        if(tokens[0] == "go") {
            parse_go(pos, search_info, tokens);
            search(pos, search_info);
        }
        if(tokens[0] == "stop") {
            search_info.stop = true;
        }
        if(tokens[0] == "d") {
            std::cout << pos.to_string() << std::endl;
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
        assert(pos.make_move(move));
    }
}

void parse_go(Position& pos, SearchInfo& search_info, const std::vector<std::string>& params) {
    const Color& us = pos.side_to_move();
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
            search_info.max_depth = stoi(params[i]);
        }
    }
    if(time == -1) {
        // go infinite
        time = std::numeric_limits<int>::max() / 2;
    }
    search_info.nodes = 0;
    search_info.start_time = current_time();
    search_info.max_time = time / 20 + increment / 2;
    search_info.stop = false;
}

} // namespace sonic