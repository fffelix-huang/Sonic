#pragma once

#include <vector>
#include <string>

#include "chess/all.h"
#include "search.h"

namespace sonic {

void uci_loop(Position& pos, SearchInfo& search_info);
void parse_position(Position& pos, SearchInfo& search_info, const std::vector<std::string>& params);
void parse_go(Position& pos, SearchInfo& search_info, const std::vector<std::string>& params);

} // namespace sonic