#pragma once

#include <string>
#include <utility>

namespace coordresolver {
    std::pair<double, double> resolveCoordinatesFromFilename(std::string filename);
    enum class CoordResolveError {
        UNEXPECTED_TOKEN,
        ANGLE_OUT_OF_RANGE,
        ANGLE_UNSPECIFIED,
        UNEXPECTED_END
    };
}