#pragma once

#include <functional>
#include <string>

namespace dir {
    void lsd(std::string dir, std::function<bool(std::string)> on_path);
}