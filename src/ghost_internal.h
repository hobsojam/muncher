#ifndef GHOST_INTERNAL_H
#define GHOST_INTERNAL_H

#include "ghost.h"
#include "player.h"

enum {
    GHOST_TUNNEL_ROW       = 14,
    GHOST_HOUSE_CENTER_COL = 13,
    GHOST_HOUSE_EXIT_ROW   = 11,
    GHOST_HOUSE_MID_ROW    = 14,
};

#define GHOST_FRIGHTEN_FLASH_SECS 2.0f

int ghost_wrap_col(int col);
int ghost_can_enter(int col, int row);
int ghost_dist_sq(int c1, int r1, int c2, int r2);
void ghost_get_target(const Ghost *g, const Player *player,
                      const Ghost ghosts[GHOST_COUNT], int *tc, int *tr);
int ghost_frighten_flashing(float t);

#ifdef MUNCHER_TEST
float ghost_internal_fright_timer(void);
#endif

#endif
