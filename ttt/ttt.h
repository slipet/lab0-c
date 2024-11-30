#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"


typedef struct {
    char *algo;
    int argc;
    char **argv;
} ttt_config;



#include "agents/mcts.h"
#include "agents/negamax.h"
#include "agents/reinforcement_learning.h"
int tic_tac_toe(int, int);
void init_ttt(int, int, void **);
