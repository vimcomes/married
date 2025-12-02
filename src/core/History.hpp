#pragma once

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

private:
    int attemptsCount_{0};
    int matchesCount_{0};
    std::vector<Attempt> attempts_;
    std::unordered_map<std::string, int> matchedCounts_;
};

}  // namespace married
