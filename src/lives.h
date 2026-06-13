#ifndef LIVES_H
#define LIVES_H

#include "player.h"
#include "ghost.h"

void handle_ghost_collision(Player *p, Ghost ghosts[], int *game_over);

#endif
