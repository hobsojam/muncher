#include "raylib.h"
#include "map.h"
#include "player.h"

#define SCREEN_W     (MAP_COLS * TILE_SIZE)
#define SCREEN_H     (MAP_ROWS * TILE_SIZE + 40)
#define MAP_OFFSET_Y 40

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Muncher");
    SetTargetFPS(60);

    map_init();

    Player player;
    player_init(&player);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        player_update(&player, dt);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("MUNCHER", 10, 10, 20, YELLOW);
            DrawText(TextFormat("SCORE: %d", player.score), 200, 10, 20, WHITE);
            map_draw(0, MAP_OFFSET_Y);
            player_draw(&player, 0, MAP_OFFSET_Y);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
