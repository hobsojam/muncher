#include "player.h"
#include "player_internal.h"
#include "map.h"
#include "raylib.h"
#include "audio.h"

#define PLAYER_START_COL 14
#define PLAYER_START_ROW 29
#define PLAYER_SPEED     8.0f
#define MAX_TILE_STEPS_PER_UPDATE 8

int player_wrap_col(int col) {
    return (col + MAP_COLS) % MAP_COLS;
}

int player_can_enter(int col, int row) {
    if (row < 0 || row >= MAP_ROWS) return 0;
    if (col < 0 || col >= MAP_COLS) return row == PLAYER_TUNNEL_ROW;
    return map[row][col] != TILE_WALL && map[row][col] != TILE_DOOR;
}

void player_init(Player *p) {
    p->col = PLAYER_START_COL;
    p->row = PLAYER_START_ROW;
    p->prev_col = p->col;
    p->prev_row = p->row;
    p->dir_col = 0;
    p->dir_row = 0;
    p->next_dir_col = 0;
    p->next_dir_row = 0;
    p->move_t = 0.0f;
    p->speed = PLAYER_SPEED;
    p->moved     = 0;
    p->score     = 0;
    p->ate_power = 0;
    p->lives       = 3;
    p->dead        = 0;
    p->ghost_chain = 0;
    map[p->row][p->col] = TILE_EMPTY;
}

void player_respawn(Player *p) {
    p->col          = PLAYER_START_COL;
    p->row          = PLAYER_START_ROW;
    p->prev_col     = p->col;
    p->prev_row     = p->row;
    p->dir_col      = 0;
    p->dir_row      = 0;
    p->next_dir_col = 0;
    p->next_dir_row = 0;
    p->move_t       = 0.0f;
    p->moved        = 0;
    p->ate_power    = 0;
    p->dead         = 0;
    p->ghost_chain  = 0;
}

void player_update(Player *p, float dt) {
    p->moved = 0;
    if (IsKeyDown(KEY_RIGHT)) { p->next_dir_col =  1; p->next_dir_row =  0; }
    if (IsKeyDown(KEY_LEFT))  { p->next_dir_col = -1; p->next_dir_row =  0; }
    if (IsKeyDown(KEY_DOWN))  { p->next_dir_col =  0; p->next_dir_row =  1; }
    if (IsKeyDown(KEY_UP))    { p->next_dir_col =  0; p->next_dir_row = -1; }

    if (p->dir_col == 0 && p->dir_row == 0) {
        if (player_can_enter(p->col + p->next_dir_col, p->row + p->next_dir_row)) {
            p->dir_col = p->next_dir_col;
            p->dir_row = p->next_dir_row;
        }
        return;
    }

    p->move_t += p->speed * dt;
    int steps = 0;
    while (p->move_t >= 1.0f && steps < MAX_TILE_STEPS_PER_UPDATE) {
        p->move_t -= 1.0f;
        p->prev_col = p->col;
        p->prev_row = p->row;
        p->col += p->dir_col;
        p->row += p->dir_row;
        p->col = player_wrap_col(p->col);
        p->moved = 1;
        steps++;

        p->ate_power = 0;
        if (map[p->row][p->col] == TILE_DOT) {
            map[p->row][p->col] = TILE_EMPTY;
            p->score += 1;
            audio_play_chomp();
        } else if (map[p->row][p->col] == TILE_POWER) {
            map[p->row][p->col] = TILE_EMPTY;
            p->score       += 10;
            p->ate_power    = 1;
            p->ghost_chain  = 0;
            audio_play_power();
        }

        if (player_can_enter(p->col + p->next_dir_col, p->row + p->next_dir_row)) {
            p->dir_col = p->next_dir_col;
            p->dir_row = p->next_dir_row;
        } else if (!player_can_enter(p->col + p->dir_col, p->row + p->dir_row)) {
            p->dir_col = 0;
            p->dir_row = 0;
            p->move_t = 0.0f;
            break;
        }
    }

    if (p->move_t >= 1.0f) {
        p->move_t = 0.0f;
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
