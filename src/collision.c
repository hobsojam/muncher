#include "collision.h"

int player_on_ghost(const Player *p, const Ghost *g) {
    return p->col == g->col && p->row == g->row;
}

void handle_collision(Player *p, Ghost ghosts[]) {
    for (int i = 0; i < GHOST_COUNT; i++) {
        if (!player_on_ghost(p, &ghosts[i])) continue;
        if (ghosts[i].mode == GMODE_FRIGHTENED) {
            p->score += 200;
            ghost_respawn(&ghosts[i]);
        } else {
            p->dead = 1;
        }
    }
}
