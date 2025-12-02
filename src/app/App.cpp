#include "app/App.h"

#include "core/DesireEngine.h"
#include "core/Input.h"
#include "ui/Ui.h"

#include <algorithm>
#include <chrono>
#include <curses.h>
#include <thread>

App::App()
    : marina_("Marina"),
      roman_("Roman"),
      worker1_(desire_worker, std::ref(marina_), std::ref(shared_), true),
      worker2_(desire_worker, std::ref(roman_), std::ref(shared_), false) {}

App::~App() {
    worker1_.request_stop();
    worker2_.request_stop();
}

int App::run() {
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
            ++attemptsCount_;
            bool match = (d1 == d2);
            if (match && matchCount_ < kTargetMatches) {
                ++matchCount_;
                ++matchedCounts_[d1];
                tickMs_ = static_cast<int>(kGeneratorTick.count());
            } else if (!match) {
                tickMs_ = std::max(50, tickMs_ - 50);
            }
            shared_.tickMs.store(tickMs_);
            attempts_.push_back(Attempt{attemptsCount_, d1, d2, match});
            if (attempts_.size() > static_cast<std::size_t>(kRecentLimit)) {
                attempts_.erase(attempts_.begin());
            }
        }

        render_frame(attemptsCount_, matchCount_, d1, d2, attempts_, tickMs_);
        if (matchCount_ >= kTargetMatches) {
            running = false;
        } else {
            running = handle_input();
        }
        std::this_thread::sleep_for(kUiTick);
    }

    if (matchCount_ >= kTargetMatches) {
        nodelay(stdscr, FALSE);
        std::vector<std::pair<std::string, int>> top;
        top.reserve(matchedCounts_.size());
        for (const auto& kv : matchedCounts_) {
            top.emplace_back(kv.first, kv.second);
        }
        std::sort(top.begin(), top.end(), [](const auto& a, const auto& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
        });
        draw_summary(top, attemptsCount_);
        getch();
    }

    return 0;
}
