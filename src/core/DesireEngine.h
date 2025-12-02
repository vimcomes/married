#pragma once

#include <stop_token>

struct Human;
struct SharedState;

void desire_worker(std::stop_token st, Human& human, SharedState& shared, bool first);
