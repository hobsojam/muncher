#ifndef COLLISION_H
#define COLLISION_H

#include "player.h"
#include "ghost.h"

int  player_on_ghost(const Player *p, const Ghost *g);
void handle_collision(Player *p, Ghost ghosts[]);

#endif
