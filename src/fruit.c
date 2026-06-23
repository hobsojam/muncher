#include "fruit.h"
#include "map.h"
#include "raylib.h"
#include "audio.h"

#define FRUIT_SECS       8.0f
#define FRUIT_TYPE_COUNT 6

static const int fruit_scores[FRUIT_TYPE_COUNT] = {
    100,   /* FRUIT_TYPE_CHERRY     */
    300,   /* FRUIT_TYPE_STRAWBERRY */
    500,   /* FRUIT_TYPE_PEACH      */
    700,   /* FRUIT_TYPE_PRETZEL    */
    1000,  /* FRUIT_TYPE_APPLE      */
    2000   /* FRUIT_TYPE_GRAPES     */
};

void fruit_init(Fruit *f, int level) {
    FruitType type = (FruitType)(level - 1);
    if (type >= FRUIT_TYPE_COUNT) type = FRUIT_TYPE_GRAPES;
    if (type < FRUIT_TYPE_CHERRY)  type = FRUIT_TYPE_CHERRY;

    f->col         = FRUIT_COL;
    f->row         = FRUIT_ROW;
    f->active      = 0;
    f->eaten       = 0;
    f->timer       = 0.0f;
    f->type        = type;
    f->score       = fruit_scores[type];
    f->popup_timer = 0.0f;
    f->popup_col   = FRUIT_COL;
    f->popup_row   = FRUIT_ROW;
}

void fruit_update(Fruit *f, Player *p, int dots_remaining, int total_dots, float dt) {
    if (!f->active && !f->eaten && total_dots > 0 && dots_remaining <= total_dots / 2) {
        f->active = 1;
        f->timer  = FRUIT_SECS;
    }
    if (f->popup_timer > 0.0f) {
        f->popup_timer -= dt;
        if (f->popup_timer < 0.0f) f->popup_timer = 0.0f;
    }
    if (!f->active) return;
    if (p->col == f->col && p->row == f->row) {
        f->popup_col   = f->col;
        f->popup_row   = f->row;
        f->popup_timer = 0.8f;
        p->score      += f->score;
        f->active      = 0;
        f->eaten       = 1;
        audio_play_fruit_eat();
        return;
    }
    f->timer -= dt;
    if (f->timer <= 0.0f) {
        f->active = 0;
        f->timer  = 0.0f;
    }
}

static const Color fruit_colors[FRUIT_TYPE_COUNT] = {
    {220, 20,  60,  255}, /* FRUIT_TYPE_CHERRY     — red         */
    {255, 105, 180, 255}, /* FRUIT_TYPE_STRAWBERRY — hot pink    */
    {255, 200, 100, 255}, /* FRUIT_TYPE_PEACH      — orange-yel  */
    {205, 133, 63,  255}, /* FRUIT_TYPE_PRETZEL    — brown       */
    {50,  205, 50,  255}, /* FRUIT_TYPE_APPLE      — green       */
    {148, 0,   211, 255}  /* FRUIT_TYPE_GRAPES     — purple      */
};

void fruit_draw(const Fruit *f, int offset_x, int offset_y) {
    if (f->active) {
        int px = (int)((float)f->col * TILE_SIZE + TILE_SIZE / 2.0f) + offset_x;
        int py = (int)((float)f->row * TILE_SIZE + TILE_SIZE / 2.0f) + offset_y;
        Color c = fruit_colors[f->type];
        DrawCircle(px, py, TILE_SIZE / 2 - 2, c);
    }
    if (f->popup_timer > 0.0f) {
        float progress  = 1.0f - (f->popup_timer / 0.8f);
        int   fx = (int)((float)f->popup_col * TILE_SIZE + TILE_SIZE / 2.0f) + offset_x - 12;
        float fy = (float)f->popup_row * TILE_SIZE + offset_y - 4.0f
                   - progress * (float)TILE_SIZE * 1.5f;
        unsigned char alpha = (unsigned char)(255.0f * (1.0f - progress));
        DrawText(TextFormat("+%d", f->score), fx, (int)fy, 14, (Color){255, 255, 255, alpha});
    }
}
