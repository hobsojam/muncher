#include "lives.h"

void handle_ghost_collision(Player *p, Ghost ghosts[], int *game_over) {
    for (int i = 0; i < GHOST_COUNT; i++) {
        if (ghosts[i].mode == GMODE_FRIGHTENED) continue;
        if (p->col != ghosts[i].col || p->row != ghosts[i].row) continue;
        p->lives--;
        if (p->lives <= 0) {
            p->lives = 0;   /* clamp — never let lives go negative */
            *game_over = 1;
        } else {
            player_respawn(p);
            ghosts_init(ghosts);
        }
        break;
    }
}
