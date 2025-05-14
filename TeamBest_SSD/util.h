#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

namespace BEST_UTILS {
    inline std::string Trim(const std::string& str) {
        auto begin = std::find_if_not(str.begin(), str.end(), ::isspace);
        auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();

        if (begin >= end)
            return "";

        return std::string(begin, end);
    }

    inline void ToUpper(char & input) {
        input = std::toupper(input);
    }

    inline std::string ToUpper(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    inline std::vector<std::string> StringTokenizer(
        const std::string& inStr, char delimiter = ' ') {
        std::vector<std::string> tokens;
        std::string token;

        for (char ch : inStr) {
            if (ch == delimiter) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }
            else {
                token += ch;
            }
        }

        if (!token.empty()) {
            tokens.push_back(token);
        }

        return tokens;
    }


    inline std::pair<int, int> getMinMax(std::vector<int> vec) {
        std::sort(vec.begin(), vec.end());
        int min = vec[0];
        int max = vec[3];
        return { min, max };
    }

    inline bool IsNumericOnly(const std::string& str) {
        if (str.empty()) return false;
        for (char ch : str) {
            if (!std::isdigit(static_cast<unsigned char>(ch))) {
                return false;
            }
        }
        return true;
    }
}
