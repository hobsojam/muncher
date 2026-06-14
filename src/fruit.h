#ifndef FRUIT_H
#define FRUIT_H

#include "player.h"

#define FRUIT_COL 13
#define FRUIT_ROW 17

typedef struct {
    int   col;
    int   row;
    int   active;
    int   eaten;
    float timer;
    int   score;
    float popup_timer;
    int   popup_col;
    int   popup_row;
} Fruit;

void fruit_init(Fruit *f);
void fruit_update(Fruit *f, Player *p, int dots_remaining, int total_dots, float dt);
void fruit_draw(const Fruit *f, int offset_x, int offset_y);

#endif
