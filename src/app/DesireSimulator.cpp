#include "app/DesireSimulator.hpp"

#include <algorithm>
#include <chrono>
#include <thread>

#include "core/DesireList.hpp"
#include "ui/HudRenderer.hpp"
#include "ui/Terminal.hpp"

namespace married {

DesireSimulator::DesireSimulator() = default;

DesireSimulator::~DesireSimulator() {
    if (worker1_.joinable()) worker1_.request_stop();
    if (worker2_.joinable()) worker2_.request_stop();
}

void DesireSimulator::startWorkers() {
    shared_.tickMs.store(tickMs_);
    worker1_ = std::jthread([this](std::stop_token st) { desireWorker(st, marina_, true); });
    worker2_ = std::jthread([this](std::stop_token st) { desireWorker(st, roman_, false); });
}

bool DesireSimulator::handleInput() {
    char ch{};
    if (!ui::pollKey(ch)) return true;
    if (ch == 'q' || ch == 'Q') return false;
    return true;
}

void DesireSimulator::desireWorker(std::stop_token st, PersonState& person, bool first) {
    while (!st.stop_requested()) {
        std::string newDesire = randomDesire();
        {
            std::scoped_lock lock(person.mtx, shared_.mtx);
            person.desire = newDesire;
            if (first) {
                shared_.desire1 = newDesire;
                shared_.updated1 = true;
            } else {
                shared_.desire2 = newDesire;
                shared_.updated2 = true;
            }
        }
        const auto sleepMs = shared_.tickMs.load();
        std::this_thread::sleep_for(std::chrono::milliseconds{sleepMs});
    }
}

int DesireSimulator::run() {
    ui::initializeTerminal();
    startWorkers();

    bool running = true;
    while (running) {
        std::string d1;
        std::string d2;
        bool ready = false;
        {
            std::scoped_lock lock(shared_.mtx);
            if (shared_.updated1 && shared_.updated2) {
                d1 = shared_.desire1;
                d2 = shared_.desire2;
                shared_.updated1 = shared_.updated2 = false;
                ready = true;
            } else {
                d1 = shared_.desire1;
                d2 = shared_.desire2;
            }
        }

        if (ready) {
            bool match = (d1 == d2);
            history_.addAttempt(d1, d2, match);
            if (match && history_.matchesCount() < kTargetMatches) {
                tickMs_ = static_cast<int>(kGeneratorTick.count());
            } else if (!match) {
                tickMs_ = std::max(50, tickMs_ - 50);
            }
            shared_.tickMs.store(tickMs_);
        }

        ui::renderFrame(history_, d1, d2, tickMs_);
        if (history_.reachedTarget()) {
            running = false;
        } else {
            running = handleInput();
        }
        std::this_thread::sleep_for(kUiTick);
    }

    if (history_.reachedTarget()) {
        ui::setBlocking(true);
        ui::renderSummary(history_);
        ui::waitKey();
    }

    ui::shutdownTerminal();
    return 0;
}

}  // namespace married
