#include "version.h"

#include <string>

std::string version_to_string() {
    return std::to_string(SONIC_VERSION_MAJOR) + "." + std::to_string(SONIC_VERSION_MINOR);
}