#include "lives.h"

void handle_ghost_collision(Player *p, Ghost ghosts[], int *game_over) {
    if (!p->dead) return;
    p->dead = 0;
    p->lives--;
    if (p->lives <= 0) {
        p->lives = 0;
        *game_over = 1;
    } else {
        player_respawn(p);
        ghosts_init(ghosts);
    }
}
