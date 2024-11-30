#pragma once

#define ITERATIONS 100000
#define EXPLORATION_FACTOR sqrt(2)

int mcts(const char *const table, const char *const player);
