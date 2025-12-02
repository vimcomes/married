#pragma once

#include <atomic>
#include <mutex>
#include <string>

#include "core/Types.h"

struct SharedState {
    std::mutex mtx;
    std::string desire1;
    std::string desire2;
    bool updated1{false};
    bool updated2{false};
    std::atomic<int> tickMs{static_cast<int>(kGeneratorTick.count())};
};
