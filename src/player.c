#include "player.h"
#include "map.h"
#include "raylib.h"

#define PLAYER_START_COL 14
#define PLAYER_START_ROW 29
#define PLAYER_SPEED     8.0f

static int can_enter(int col, int row) {
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return 0;
    return map[row][col] != TILE_WALL && map[row][col] != TILE_DOOR;
}

void player_init(Player *p) {
    p->col = PLAYER_START_COL;
    p->row = PLAYER_START_ROW;
    p->dir_col = 0;
    p->dir_row = 0;
    p->next_dir_col = 0;
    p->next_dir_row = 0;
    p->move_t = 0.0f;
    p->speed = PLAYER_SPEED;
    map[p->row][p->col] = TILE_EMPTY;
}

void player_update(Player *p, float dt) {
    if (IsKeyDown(KEY_RIGHT)) { p->next_dir_col =  1; p->next_dir_row =  0; }
    if (IsKeyDown(KEY_LEFT))  { p->next_dir_col = -1; p->next_dir_row =  0; }
    if (IsKeyDown(KEY_DOWN))  { p->next_dir_col =  0; p->next_dir_row =  1; }
    if (IsKeyDown(KEY_UP))    { p->next_dir_col =  0; p->next_dir_row = -1; }

    if (p->dir_col == 0 && p->dir_row == 0) {
        if (can_enter(p->col + p->next_dir_col, p->row + p->next_dir_row)) {
            p->dir_col = p->next_dir_col;
            p->dir_row = p->next_dir_row;
        }
        return;
    }

    p->move_t += p->speed * dt;
    if (p->move_t >= 1.0f) {
        p->move_t -= 1.0f;
        p->col += p->dir_col;
        p->row += p->dir_row;

        if (map[p->row][p->col] == TILE_DOT || map[p->row][p->col] == TILE_POWER) {
            map[p->row][p->col] = TILE_EMPTY;
        }

        if (can_enter(p->col + p->next_dir_col, p->row + p->next_dir_row)) {
            p->dir_col = p->next_dir_col;
            p->dir_row = p->next_dir_row;
        } else if (!can_enter(p->col + p->dir_col, p->row + p->dir_row)) {
            p->dir_col = 0;
            p->dir_row = 0;
            p->move_t = 0.0f;
        }
    }
}

void player_draw(const Player *p, int offset_x, int offset_y) {
    float t = p->move_t;
    float px = ((float)p->col + (float)p->dir_col * t) * TILE_SIZE + TILE_SIZE / 2.0f + offset_x;
    float py = ((float)p->row + (float)p->dir_row * t) * TILE_SIZE + TILE_SIZE / 2.0f + offset_y;
    float radius = (float)TILE_SIZE / 2.0f - 1.0f;

    float mouth_dir = 0.0f;
    if      (p->dir_col ==  1) mouth_dir =   0.0f;
    else if (p->dir_col == -1) mouth_dir = 180.0f;
    else if (p->dir_row ==  1) mouth_dir =  90.0f;
    else if (p->dir_row == -1) mouth_dir = 270.0f;

    DrawCircleSector(
        (Vector2){px, py},
        radius,
        mouth_dir + 30.0f,
        mouth_dir + 330.0f,
        32,
        YELLOW
    );
}
