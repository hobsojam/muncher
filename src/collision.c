#include "collision.h"

int player_on_ghost(const Player *p, const Ghost *g) {
    return p->col == g->col && p->row == g->row;
}

static int player_crossed_ghost(const Player *p, const Ghost *g) {
    return p->moved && g->moved &&
           p->prev_col == g->col && p->prev_row == g->row &&
           g->prev_col == p->col && g->prev_row == p->row;
}

void handle_collision(Player *p, Ghost ghosts[]) {
    for (int i = 0; i < GHOST_COUNT; i++) {
        if (!player_on_ghost(p, &ghosts[i]) && !player_crossed_ghost(p, &ghosts[i])) continue;
        if (ghosts[i].mode == GMODE_FRIGHTENED) {
            p->score += 200;
            ghost_respawn(&ghosts[i]);
        } else {
            p->dead = 1;
        }
    }
}
