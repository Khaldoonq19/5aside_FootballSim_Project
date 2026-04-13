#pragma once
#include <string>
#include <sstream>
#include <iomanip>

struct Score {
    int home{ 0 };
    int away{ 0 };
};

struct Vec2 {
    int x{ 0 };
    int y{ 0 };
};

inline std::string padRight(const std::string& s, int w) {
    std::ostringstream oss;
    oss << std::left << std::setw(w) << s;
    return oss.str();
}
