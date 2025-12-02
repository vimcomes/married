#pragma once

#include <chrono>
#include <string>

inline constexpr int kScreenWidth = 80;
inline constexpr int kScreenHeight = 24;
inline constexpr std::chrono::milliseconds kGeneratorTick{500};
inline constexpr std::chrono::milliseconds kUiTick{200};
inline constexpr int kRecentLimit = 8;
inline constexpr int kTargetMatches = 10;

struct Attempt {
    int number{};
    std::string d1;
    std::string d2;
    bool match{};
};
