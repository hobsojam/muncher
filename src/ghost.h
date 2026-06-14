#ifndef GHOST_H
#define GHOST_H

#include "raylib.h"
#include "player.h"

#define GHOST_COUNT 4

typedef enum {
    GHOST_BLINKY = 0,
    GHOST_PINKY,
    GHOST_INKY,
    GHOST_CLYDE,
} GhostId;

typedef enum {
    GMODE_SCATTER    = 0,
    GMODE_CHASE      = 1,
    GMODE_FRIGHTENED = 2,
} GhostMode;

typedef struct {
    GhostId   id;
    int       col, row;
    int       dir_col, dir_row;
    float     move_t;
    float     speed;
    GhostMode mode;
    int       scatter_col, scatter_row;
    Color     color;
} Ghost;

void ghosts_init(Ghost ghosts[GHOST_COUNT]);
void ghosts_update(Ghost ghosts[GHOST_COUNT], const Player *player, float dt);
void ghosts_draw(const Ghost ghosts[GHOST_COUNT], int offset_x, int offset_y);
void ghosts_frighten(Ghost ghosts[GHOST_COUNT]);
void ghost_respawn(Ghost *g);

#endif
