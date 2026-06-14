#ifndef GHOST_INTERNAL_H
#define GHOST_INTERNAL_H

#include "ghost.h"
#include "player.h"

enum {
    GHOST_TUNNEL_ROW = 14,
};

int ghost_wrap_col(int col);
int ghost_can_enter(int col, int row);
int ghost_dist_sq(int c1, int r1, int c2, int r2);
void ghost_get_target(const Ghost *g, const Player *player,
                      const Ghost ghosts[GHOST_COUNT], int *tc, int *tr);

#endif
