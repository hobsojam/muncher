#include "fruit.h"
#include "map.h"
#include "raylib.h"

#define FRUIT_SECS 8.0f
#define FRUIT_SCORE 100

void fruit_init(Fruit *f) {
    f->col    = FRUIT_COL;
    f->row    = FRUIT_ROW;
    f->active = 0;
    f->eaten  = 0;
    f->timer  = 0.0f;
    f->score  = FRUIT_SCORE;
}

void fruit_update(Fruit *f, Player *p, int dots_remaining, int total_dots, float dt) {
    if (!f->active && !f->eaten && total_dots > 0 && dots_remaining <= total_dots / 2) {
        f->active = 1;
        f->timer  = FRUIT_SECS;
    }
    if (!f->active) return;
    if (p->col == f->col && p->row == f->row) {
        p->score += f->score;
        f->active = 0;
        f->eaten  = 1;
        return;
    }
    f->timer -= dt;
    if (f->timer <= 0.0f) {
        f->active = 0;
        f->timer  = 0.0f;
    }
}

void fruit_draw(const Fruit *f, int offset_x, int offset_y) {
    if (!f->active) return;
    int px = (int)((float)f->col * TILE_SIZE + TILE_SIZE / 2.0f) + offset_x;
    int py = (int)((float)f->row * TILE_SIZE + TILE_SIZE / 2.0f) + offset_y;
    DrawCircle(px, py, TILE_SIZE / 2 - 2, (Color){220, 20, 60, 255});
}
