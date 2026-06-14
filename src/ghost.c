#include "ghost.h"
#include "player.h"
#include "map.h"
#include "raylib.h"
#include <stdlib.h>

#define SPEED_NORMAL     6.5f
#define SPEED_FRIGHTENED 4.0f
#define FRIGHTENED_SECS  8.0f

// Mode schedule: alternating scatter/chase, then permanent chase
static const float      MODE_DURATIONS[] = { 7.0f, 20.0f, 7.0f, 20.0f, 5.0f, 20.0f, 5.0f, 99999.0f };
static const GhostMode  MODE_SEQUENCE[]  = {
    GMODE_SCATTER, GMODE_CHASE, GMODE_SCATTER, GMODE_CHASE,
    GMODE_SCATTER, GMODE_CHASE, GMODE_SCATTER, GMODE_CHASE
};
#define MODE_COUNT 8

static int       global_index  = 0;
static float     global_timer  = 0.0f;
static GhostMode global_mode   = GMODE_SCATTER;
static float     fright_timer  = 0.0f;

// Up, left, down, right — classic Pac-Man priority order
static const int DIRS[4][2] = { {0,-1}, {-1,0}, {0,1}, {1,0} };

#define TUNNEL_ROW 14

static int ghost_wrap_col(int col) {
    return (col + MAP_COLS) % MAP_COLS;
}

static int ghost_can_enter(int col, int row) {
    if (row < 0 || row >= MAP_ROWS) return 0;
    if (col < 0 || col >= MAP_COLS) return row == TUNNEL_ROW;
    return map[row][col] != TILE_WALL && map[row][col] != TILE_DOOR;
}

static int dist_sq(int c1, int r1, int c2, int r2) {
    int dc = c2 - c1, dr = r2 - r1;
    return dc*dc + dr*dr;
}

// Pick the direction (no reversing) that minimises distance to (tc, tr).
// Falls back to reversing if no other valid direction exists.
static void choose_dir(Ghost *g, int tc, int tr) {
    int rev_dc = -g->dir_col, rev_dr = -g->dir_row;
    int best_dc = 0, best_dr = 0, best_d = -1;

    for (int i = 0; i < 4; i++) {
        int dc = DIRS[i][0], dr = DIRS[i][1];
        if (dc == rev_dc && dr == rev_dr) continue;
        if (!ghost_can_enter(g->col + dc, g->row + dr)) continue;
        int d = dist_sq(g->col + dc, g->row + dr, tc, tr);
        if (best_d < 0 || d < best_d) { best_d = d; best_dc = dc; best_dr = dr; }
    }
    // fallback: allow reversing if every other direction is blocked
    if (best_d < 0 && ghost_can_enter(g->col + rev_dc, g->row + rev_dr)) {
        best_dc = rev_dc; best_dr = rev_dr;
    }
    g->dir_col = best_dc;
    g->dir_row = best_dr;
}

// Pick a random valid direction (no reversing) — used when frightened.
static void choose_dir_random(Ghost *g) {
    int rev_dc = -g->dir_col, rev_dr = -g->dir_row;
    int valid[4][2];
    int n = 0;

    for (int i = 0; i < 4; i++) {
        int dc = DIRS[i][0], dr = DIRS[i][1];
        if (dc == rev_dc && dr == rev_dr) continue;
        if (!ghost_can_enter(g->col + dc, g->row + dr)) continue;
        valid[n][0] = dc; valid[n][1] = dr; n++;
    }
    if (n > 0) {
        int pick = rand() % n;
        g->dir_col = valid[pick][0];
        g->dir_row = valid[pick][1];
    }
}

static void get_target(const Ghost *g, const Player *player,
                       const Ghost ghosts[GHOST_COUNT], int *tc, int *tr) {
    if (g->mode == GMODE_SCATTER) {
        *tc = g->scatter_col; *tr = g->scatter_row; return;
    }
    switch (g->id) {
        case GHOST_BLINKY:
            *tc = player->col; *tr = player->row;
            break;
        case GHOST_PINKY:
            *tc = player->col + player->dir_col * 4;
            *tr = player->row + player->dir_row * 4;
            break;
        case GHOST_INKY: {
            int ref_c = player->col + player->dir_col * 2;
            int ref_r = player->row + player->dir_row * 2;
            int bc = ghosts[GHOST_BLINKY].col, br = ghosts[GHOST_BLINKY].row;
            *tc = ref_c + (ref_c - bc);
            *tr = ref_r + (ref_r - br);
            break;
        }
        case GHOST_CLYDE:
            if (dist_sq(g->col, g->row, player->col, player->row) > 64) {
                *tc = player->col; *tr = player->row;
            } else {
                *tc = g->scatter_col; *tr = g->scatter_row;
            }
            break;
    }
}

void ghosts_init(Ghost ghosts[GHOST_COUNT]) {
    global_index = 0;
    global_timer = 0.0f;
    global_mode  = GMODE_SCATTER;
    fright_timer = 0.0f;

    // Start spread across the corridor above the ghost house (row 11, cols 9-18 are open)
    ghosts[GHOST_BLINKY] = (Ghost){ GHOST_BLINKY,  9, 11,  1, 0, 0.0f, SPEED_NORMAL, GMODE_SCATTER, 25,  0, RED,     0, 0, 0 };
    ghosts[GHOST_PINKY]  = (Ghost){ GHOST_PINKY,  18, 11, -1, 0, 0.0f, SPEED_NORMAL, GMODE_SCATTER,  2,  0, PINK,    0, 0, 0 };
    ghosts[GHOST_INKY]   = (Ghost){ GHOST_INKY,   11, 11,  1, 0, 0.0f, SPEED_NORMAL, GMODE_SCATTER, 27, 30, SKYBLUE, 0, 0, 0 };
    ghosts[GHOST_CLYDE]  = (Ghost){ GHOST_CLYDE,  16, 11, -1, 0, 0.0f, SPEED_NORMAL, GMODE_SCATTER,  0, 30, ORANGE,  0, 0, 0 };
}

void ghosts_update(Ghost ghosts[GHOST_COUNT], const Player *player, float dt) {
    // Advance global mode timer
    global_timer += dt;
    if (global_timer >= MODE_DURATIONS[global_index]) {
        global_timer = 0.0f;
        if (global_index < MODE_COUNT - 1) global_index++;
        global_mode = MODE_SEQUENCE[global_index];
        for (int i = 0; i < GHOST_COUNT; i++) {
            if (ghosts[i].mode != GMODE_FRIGHTENED) {
                ghosts[i].mode = global_mode;
                // Reverse direction on mode switch
                ghosts[i].dir_col = -ghosts[i].dir_col;
                ghosts[i].dir_row = -ghosts[i].dir_row;
            }
        }
    }

    // Advance frightened timer
    if (fright_timer > 0.0f) {
        fright_timer -= dt;
        if (fright_timer <= 0.0f) {
            fright_timer = 0.0f;
            for (int i = 0; i < GHOST_COUNT; i++) {
                if (ghosts[i].mode == GMODE_FRIGHTENED) {
                    ghosts[i].mode = global_mode;
                    ghosts[i].speed = SPEED_NORMAL;
                }
            }
        }
    }

    // Tick eat-flash timers
    for (int i = 0; i < GHOST_COUNT; i++) {
        if (ghosts[i].flash_timer > 0.0f) {
            ghosts[i].flash_timer -= dt;
            if (ghosts[i].flash_timer < 0.0f) ghosts[i].flash_timer = 0.0f;
        }
    }

    // Move each ghost
    for (int i = 0; i < GHOST_COUNT; i++) {
        Ghost *g = &ghosts[i];
        g->move_t += g->speed * dt;

        if (g->move_t >= 1.0f) {
            g->move_t -= 1.0f;
            g->col += g->dir_col;
            g->row += g->dir_row;
            g->col = ghost_wrap_col(g->col);

            if (g->mode == GMODE_FRIGHTENED) {
                choose_dir_random(g);
            } else {
                int tc, tr;
                get_target(g, player, ghosts, &tc, &tr);
                choose_dir(g, tc, tr);
            }
        }
    }
}

void ghosts_draw(const Ghost ghosts[GHOST_COUNT], int offset_x, int offset_y) {
    for (int i = 0; i < GHOST_COUNT; i++) {
        const Ghost *g = &ghosts[i];
        float t  = g->move_t;
        float px = ((float)g->col + (float)g->dir_col * t) * TILE_SIZE + TILE_SIZE / 2.0f + offset_x;
        float py = ((float)g->row + (float)g->dir_row * t) * TILE_SIZE + TILE_SIZE / 2.0f + offset_y;
        float r  = (float)TILE_SIZE / 2.0f - 1.0f;

        Color body = (g->mode == GMODE_FRIGHTENED) ? BLUE : g->color;

        // Body: circle head + rectangular skirt
        DrawCircle((int)px, (int)(py - r * 0.25f), (int)r, body);
        DrawRectangle((int)(px - r), (int)(py - r * 0.25f), (int)(r * 2.0f), (int)(r * 1.25f), body);

        // Eyes (hidden when frightened)
        if (g->mode != GMODE_FRIGHTENED) {
            int ex1 = (int)(px - r * 0.35f), ex2 = (int)(px + r * 0.35f);
            int ey  = (int)(py - r * 0.4f);
            DrawCircle(ex1, ey, 3, WHITE);
            DrawCircle(ex2, ey, 3, WHITE);
            DrawCircle(ex1 + g->dir_col * 2, ey + g->dir_row * 2, 2, DARKBLUE);
            DrawCircle(ex2 + g->dir_col * 2, ey + g->dir_row * 2, 2, DARKBLUE);
        }

        // Score popup when ghost is eaten
        if (g->flash_timer > 0.0f) {
            int fx = (int)((float)g->flash_col * TILE_SIZE + TILE_SIZE / 2.0f) + offset_x - 12;
            int fy = (int)((float)g->flash_row * TILE_SIZE) + offset_y - 4;
            DrawText("+200", fx, fy, 14, WHITE);
        }
    }
}

void ghost_respawn(Ghost *g) {
    g->flash_col   = g->col;
    g->flash_row   = g->row;
    g->flash_timer = 0.8f;
    /* TODO: play an eat-ghost sound here — see raylib InitAudioDevice + PlaySound */
    switch (g->id) {
        case GHOST_BLINKY: g->col =  9; g->row = 11; g->dir_col =  1; g->dir_row = 0; break;
        case GHOST_PINKY:  g->col = 18; g->row = 11; g->dir_col = -1; g->dir_row = 0; break;
        case GHOST_INKY:   g->col = 11; g->row = 11; g->dir_col =  1; g->dir_row = 0; break;
        case GHOST_CLYDE:  g->col = 16; g->row = 11; g->dir_col = -1; g->dir_row = 0; break;
    }
    g->move_t = 0.0f;
    g->speed  = SPEED_NORMAL;
    g->mode   = global_mode;
}

void ghosts_frighten(Ghost ghosts[GHOST_COUNT]) {
    fright_timer = FRIGHTENED_SECS;
    for (int i = 0; i < GHOST_COUNT; i++) {
        ghosts[i].mode    = GMODE_FRIGHTENED;
        ghosts[i].speed   = SPEED_FRIGHTENED;
        // Reverse direction immediately
        ghosts[i].dir_col = -ghosts[i].dir_col;
        ghosts[i].dir_row = -ghosts[i].dir_row;
    }
}
