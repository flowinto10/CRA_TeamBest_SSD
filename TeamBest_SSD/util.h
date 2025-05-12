#pragma once

#include <string>
#include <algorithm>
#include <cctype>

namespace BEST_UTILS {
    std::string ToUpper(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }
}
