#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include "core/History.hpp"
#include "core/Types.hpp"

namespace married {

class DesireSimulator {
public:
    DesireSimulator();
    ~DesireSimulator();

    int run();

private:
    struct PersonState {
        explicit PersonState(std::string name) : name(std::move(name)) {}
        std::string name;
        std::string desire;
        std::mutex mtx;
    };

    struct SharedState {
        std::mutex mtx;
        std::string desire1;
        std::string desire2;
        bool updated1{false};
        bool updated2{false};
        std::atomic<int> tickMs{static_cast<int>(kGeneratorTick.count())};
    };

    void startWorkers();
    bool handleInput();
    void desireWorker(std::stop_token st, PersonState& person, bool first);

    History history_;
    SharedState shared_;
    PersonState marina_{"Marina"};
    PersonState roman_{"Roman"};
    std::jthread worker1_;
    std::jthread worker2_;
    int tickMs_{static_cast<int>(kGeneratorTick.count())};
};

}  // namespace married
