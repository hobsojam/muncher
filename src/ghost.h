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
    GMODE_HOUSE      = 3,
    GMODE_EXITING    = 4,
} GhostMode;

typedef struct {
    GhostId   id;
    int       col;
    int       row;
    int       prev_col;
    int       prev_row;
    int       dir_col;
    int       dir_row;
    float     move_t;
    float     speed;
    int       moved;
    GhostMode mode;
    int       scatter_col;
    int       scatter_row;
    Color     color;
    float     flash_timer;
    int       flash_tile;
    float     release_timer;
    int       eat_score;
    float     exit_flash_timer;
    int       shy_radius;
} Ghost;

void ghosts_init(Ghost ghosts[GHOST_COUNT]);
void ghosts_init_level(Ghost ghosts[GHOST_COUNT], int level);
void ghosts_update(Ghost ghosts[GHOST_COUNT], const Player *player, float dt);
void ghosts_draw(const Ghost ghosts[GHOST_COUNT], int offset_x, int offset_y);
void ghosts_frighten(Ghost ghosts[GHOST_COUNT]);
void ghost_respawn(Ghost *g);

#endif
