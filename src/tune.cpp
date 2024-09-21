#include "tune.h"

#include <string>

#include "ucioption.h"

namespace sonic {

#define TUNE_PARAM(name, value, min, max) TunableParam name(#name, value, min, max)

TUNE_PARAM(DELTA_MARGIN, 850, 100, 1000);
TUNE_PARAM(RFP_BASE, 250, 50, 900);

extern OptionsMap options;

TunableParam::TunableParam(std::string name_, int value_, int min_, int max_) : name(name_), value(value_), min(min_), max(max_) {
    options.add_tune_option(*this);
}

} // namespace sonic