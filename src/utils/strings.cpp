#include "strings.h"

#include <istream>
#include <vector>
#include <sstream>
#include <string>

namespace sonic {

std::vector<std::string> split_string(const std::string& s, char delim) {
    std::istringstream       in(s);
    std::vector<std::string> tokens;
    std::string              token;
    while (std::getline(in, token, delim))
    {
        tokens.push_back(token);
    }
    return tokens;
}

}  // namespace sonic