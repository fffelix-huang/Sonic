#pragma once

#include <string>
#include <vector>

#include "ucioption.h"

namespace sonic {

extern OptionsMap options;

struct TunableParam {
    TunableParam() {}
    TunableParam(std::string name_, int value_, int min_, int max_) : name(name_), value(value_), min(min_), max(max_) {
        options.add_tune_option(name, &value, min, max);
    }

    std::string name;
    int value;
    int min, max;

    operator int() const { return value; }
};

#define TUNE_PARAM(name, value, min, max) TunableParam name(#name, value, min, max)

TUNE_PARAM(DELTA_MARGIN, 850, 100, 3000);
TUNE_PARAM(RFP_BASE, 250, 50, 900);
TUNE_PARAM(FP_BASE, 175, 50, 900);

} // namespace sonic