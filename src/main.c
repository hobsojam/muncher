#include "raylib.h"
#include "map.h"
#include "player.h"
#include "ghost.h"
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

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        player_update(&player, dt);
        if (player.ate_power) ghosts_frighten(ghosts);
        ghosts_update(ghosts, &player, dt);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("MUNCHER", 10, 10, 20, YELLOW);
            DrawText(TextFormat("SCORE: %d", player.score), 200, 10, 20, WHITE);
            map_draw(0, MAP_OFFSET_Y);
            player_draw(&player, 0, MAP_OFFSET_Y);
            ghosts_draw(ghosts, 0, MAP_OFFSET_Y);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
