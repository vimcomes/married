#pragma once

#include <string>
#include <thread>
#include <vector>
#include <unordered_map>

#include "core/Human.h"
#include "core/SharedState.h"
#include "core/Types.h"

class App {
public:
    App();
    ~App();

    int run();

private:
    SharedState shared_;
    Human marina_;
    Human roman_;
    std::jthread worker1_;
    std::jthread worker2_;

    int attemptsCount_{0};
    int matchCount_{0};
    int tickMs_{static_cast<int>(kGeneratorTick.count())};
    std::vector<Attempt> attempts_;
    std::unordered_map<std::string, int> matchedCounts_;
};
