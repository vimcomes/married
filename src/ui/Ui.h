#pragma once

#include <string>
#include <vector>

#include "core/Types.h"

void init_ncurses();
void shutdown_ncurses();

void render_frame(int attemptsCount,
                  int matchCount,
                  const std::string& d1,
                  const std::string& d2,
                  const std::vector<Attempt>& attempts,
                  int tickMs);

void draw_summary(const std::vector<std::pair<std::string, int>>& topMatches,
                  int attemptsCount);
