#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core/Types.hpp"

namespace married {

class History {
public:
    void addAttempt(const std::string& d1, const std::string& d2, bool match);

    int attemptsCount() const { return attemptsCount_; }
    int matchesCount() const { return matchesCount_; }
    bool reachedTarget() const { return matchesCount_ >= kTargetMatches; }

    const std::vector<Attempt>& attempts() const { return attempts_; }
    std::vector<std::pair<std::string, int>> topMatches() const;

    bool hasTiming() const { return matchIntervals_ > 0; }
    int fastestMs() const { return minMatchMs_; }
    int slowestMs() const { return maxMatchMs_; }
    int averageMs() const {
        return matchIntervals_ > 0 ? static_cast<int>(totalMatchMs_ / matchIntervals_) : 0;
    }

private:
    int attemptsCount_{0};
    int matchesCount_{0};
    std::vector<Attempt> attempts_;
    std::unordered_map<std::string, int> matchedCounts_;

    std::chrono::steady_clock::time_point startTime_{std::chrono::steady_clock::now()};
    std::optional<std::chrono::steady_clock::time_point> lastMatchTime_;
    int minMatchMs_{0};
    int maxMatchMs_{0};
    long long totalMatchMs_{0};
    int matchIntervals_{0};
};

}  // namespace married
