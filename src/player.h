#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int col;
    int row;
    int prev_col;
    int prev_row;
    int dir_col;
    int dir_row;
    int next_dir_col;
    int next_dir_row;
    float move_t;
    float speed;
    int moved;
    int score;
    int ate_power;  // set to 1 for one frame when a power pellet is eaten
    int lives;
    int dead;
    int ghost_chain;
    int extra_life_flags;
} Player;

void player_init(Player *p);
void player_respawn(Player *p);
void player_update(Player *p, float dt);
void player_draw(const Player *p, int offset_x, int offset_y, float death_progress);
void player_check_extra_life(Player *p);
void player_set_level_speed(Player *p, int level);

#endif
