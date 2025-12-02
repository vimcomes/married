#include "core/History.hpp"

#include <algorithm>

namespace married {

void History::addAttempt(const std::string& d1, const std::string& d2, bool match) {
    ++attemptsCount_;
    if (match && matchesCount_ < kTargetMatches) {
        ++matchesCount_;
        ++matchedCounts_[d1];
    }

    attempts_.push_back(Attempt{attemptsCount_, d1, d2, match});
    if (attempts_.size() > static_cast<std::size_t>(kRecentLimit)) {
        attempts_.erase(attempts_.begin());
    }
}

std::vector<std::pair<std::string, int>> History::topMatches() const {
    std::vector<std::pair<std::string, int>> top;
    top.reserve(matchedCounts_.size());
    for (const auto& kv : matchedCounts_) {
        top.emplace_back(kv.first, kv.second);
    }
    std::sort(top.begin(), top.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    return top;
}

}  // namespace married
