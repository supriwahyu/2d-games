#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

typedef struct {
    int score;
    int coins;
    int lives;
    bool level_won;
    bool game_over;
    bool player_bounce_pending;
    bool player_hurt_pending;
} GameState;

extern GameState g_state;

void game_state_reset(void);

#endif
