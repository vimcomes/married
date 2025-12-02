#pragma once

#include <string>

#include "core/History.hpp"

namespace married::ui {

void renderHeader(int attempts, int matches, int tickMs);
void renderProgress(int matches);
void renderCurrentDesires(const std::string& d1, const std::string& d2);
void renderStatusLine();

void renderFrame(const married::History& history,
                 const std::string& d1,
                 const std::string& d2,
                 int tickMs);

void renderSummary(const married::History& history);

}  // namespace married::ui
