#include "core/DesireEngine.h"

#include "core/Human.h"
#include "core/SharedState.h"
#include "core/Types.h"

#include <chrono>
#include <thread>

void desire_worker(std::stop_token st, Human& human, SharedState& shared, bool first) {
    while (!st.stop_requested()) {
        std::string newDesire = random_desire();
        {
            std::scoped_lock lock(human.mtx, shared.mtx);
            human.desire = newDesire;
            if (first) {
                shared.desire1 = newDesire;
                shared.updated1 = true;
            } else {
                shared.desire2 = newDesire;
                shared.updated2 = true;
            }
        }
        const auto sleepMs = shared.tickMs.load();
        std::this_thread::sleep_for(std::chrono::milliseconds{sleepMs});
    }
}
