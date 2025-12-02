#include "core/History.hpp"

#include <algorithm>

namespace married {

void History::addAttempt(const std::string& d1, const std::string& d2, bool match) {
    ++attemptsCount_;
    if (match && matchesCount_ < kTargetMatches) {
        ++matchesCount_;
        ++matchedCounts_[d1];

        const auto now = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - (lastMatchTime_.has_value() ? *lastMatchTime_ : startTime_));
        lastMatchTime_ = now;

        const int intervalMs = static_cast<int>(duration.count());
        if (matchIntervals_ == 0) {
            minMatchMs_ = maxMatchMs_ = intervalMs;
        } else {
            minMatchMs_ = std::min(minMatchMs_, intervalMs);
            maxMatchMs_ = std::max(maxMatchMs_, intervalMs);
        }
        totalMatchMs_ += intervalMs;
        ++matchIntervals_;
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
