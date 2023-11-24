#pragma once

#include <string>

namespace utils {
    void replaceAll(std::string &str, const std::string& from, const std::string& to);
    bool endsWith(const std::string& str, const std::string& suffix);
}
