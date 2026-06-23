#ifndef FRUIT_H
#define FRUIT_H

#include "player.h"

#define FRUIT_COL 13
#define FRUIT_ROW 17

typedef enum {
    FRUIT_TYPE_CHERRY = 0,
    FRUIT_TYPE_STRAWBERRY,
    FRUIT_TYPE_PEACH,
    FRUIT_TYPE_PRETZEL,
    FRUIT_TYPE_APPLE,
    FRUIT_TYPE_GRAPES
} FruitType;

typedef struct {
    int       col;
    int       row;
    int       active;
    int       eaten;
    float     timer;
    int       score;
    float     popup_timer;
    int       popup_col;
    int       popup_row;
    FruitType type;
} Fruit;

void fruit_init(Fruit *f, int level);
void fruit_update(Fruit *f, Player *p, int dots_remaining, int total_dots, float dt);
void fruit_draw(const Fruit *f, int offset_x, int offset_y);

#endif
