#pragma once

#include <string>
#include <vector>

namespace sonic {

struct TunableParam {
    TunableParam() {}
    TunableParam(std::string name_, int value_, int min_, int max_);

    std::string name;
    int value;
    int min, max;

    operator int() const { return value; }
};

extern TunableParam DELTA_MARGIN;
extern TunableParam RFP_BASE;

} // namespace sonic