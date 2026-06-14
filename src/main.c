#include "raylib.h"
#include "map.h"
#include "player.h"
#include "ghost.h"
#include "lives.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_W     (MAP_COLS * TILE_SIZE)
#define SCREEN_H     (MAP_ROWS * TILE_SIZE + 40)
#define MAP_OFFSET_Y 40

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Muncher");
    SetTargetFPS(60);
    srand((unsigned)time(NULL));

    map_init();

    Player player;
    player_init(&player);

    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);

    static int you_win   = 0;
    static int game_over = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!you_win && !game_over) {
            player_update(&player, dt);
            if (player.ate_power) ghosts_frighten(ghosts);
            ghosts_update(ghosts, &player, dt);
            handle_ghost_collision(&player, ghosts, &game_over);
            if (map_dots_remaining() == 0) you_win = 1;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("MUNCHER", 10, 10, 20, YELLOW);
            DrawText(TextFormat("SCORE: %d", player.score), 200, 10, 20, WHITE);
            DrawText(TextFormat("LIVES: %d", player.lives), 400, 10, 20, WHITE);
            map_draw(0, MAP_OFFSET_Y);
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
