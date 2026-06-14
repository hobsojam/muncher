#include "raylib.h"
#include "map.h"
#include "player.h"
#include "ghost.h"
#include "lives.h"
#include "collision.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_W          (MAP_COLS * TILE_SIZE)
#define SCREEN_H          (MAP_ROWS * TILE_SIZE + 40)
#define MAP_OFFSET_Y      40
#define DEATH_FREEZE_SECS 1.5f

static void game_update(Player *p, Ghost ghosts[], float dt,
                        float *death_timer, int *game_over, int *you_win) {
    if (*death_timer > 0.0f) {
        *death_timer -= dt;
        if (*death_timer <= 0.0f) {
            *death_timer = 0.0f;
            handle_ghost_collision(p, ghosts, game_over);
        }
        return;
    }
    player_update(p, dt);
    if (p->ate_power) ghosts_frighten(ghosts);
    ghosts_update(ghosts, p, dt);
    handle_collision(p, ghosts);
    if (p->dead) {
        *death_timer = DEATH_FREEZE_SECS;
        /* Add a death sound here — see raylib InitAudioDevice + PlaySound */
    }
    if (map_dots_remaining() == 0) *you_win = 1;
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Muncher");
    SetTargetFPS(60);
    srand((unsigned)time(NULL));

    map_init();

    Player player;
    player_init(&player);

    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);

    static int   you_win     = 0;
    static int   game_over   = 0;
    static float death_timer = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!you_win && !game_over)
            game_update(&player, ghosts, dt, &death_timer, &game_over, &you_win);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("MUNCHER", 10, 10, 20, YELLOW);
            DrawText(TextFormat("SCORE: %d", player.score), 200, 10, 20, WHITE);
            DrawText(TextFormat("LIVES: %d", player.lives), 400, 10, 20, WHITE);
            map_draw(0, MAP_OFFSET_Y);
            if (!player.dead || (int)(death_timer * 6) % 2)
                player_draw(&player, 0, MAP_OFFSET_Y);
            ghosts_draw(ghosts, 0, MAP_OFFSET_Y);
            if (you_win)
                DrawText("YOU WIN!", SCREEN_W / 2 - 80, SCREEN_H / 2 - 20, 40, YELLOW);
            if (game_over) {
                int text_w = MeasureText("GAME OVER", 40);
                DrawText("GAME OVER",
                         (SCREEN_W - text_w) / 2,
                         SCREEN_H / 2 - 20,
                         40, RED);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
