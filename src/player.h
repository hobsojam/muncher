#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int col, row;
    int dir_col, dir_row;
    int next_dir_col, next_dir_row;
    float move_t;
    float speed;
    int score;
} Player;

void player_init(Player *p);
void player_update(Player *p, float dt);
void player_draw(const Player *p, int offset_x, int offset_y);

#endif
