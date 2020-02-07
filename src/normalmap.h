#pragma once

#include <vector>

namespace normal {
    std::vector<short> generateNormalMapFromHeightMap(const std::vector<short> & data, size_t img_w, size_t x, size_t y, size_t w, size_t h);
}