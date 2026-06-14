#include "raylib.h"
#include "map.h"
#include "player.h"
#include "ghost.h"
#include "lives.h"
#include "collision.h"
#include "audio.h"
#include "fruit.h"
#include "hiscore.h"
#include <stdlib.h>
#include <time.h>

#define HISCORE_PATH      "muncher_hiscore.dat"
#define SCREEN_W          (MAP_COLS * TILE_SIZE)
#define SCREEN_H          (MAP_ROWS * TILE_SIZE + 60)
#define MAP_OFFSET_Y      60
#define DEATH_FREEZE_SECS 1.5f

typedef enum { STATE_TITLE = 0, STATE_PLAYING } GameState;

static void game_update(Player *p, Ghost ghosts[], Fruit *fruit, int total_dots,
                        float dt, float *death_timer, int *game_over, int *you_win) {
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
    fruit_update(fruit, p, map_dots_remaining(), total_dots, dt);
    if (p->dead) {
        *death_timer = DEATH_FREEZE_SECS;
        audio_play_death();
    }
    if (map_dots_remaining() == 0) *you_win = 1;
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Muncher");
    SetTargetFPS(60);
    audio_init();
    srand((unsigned)time(NULL));

    int level = 1;
    if (!map_generate(level)) {
        audio_close();
        CloseWindow();
        return 1;
    }

    Player player;
    player_init(&player);

    Ghost ghosts[GHOST_COUNT];
    ghosts_init_level(ghosts, level);

    Fruit fruit;
    fruit_init(&fruit);
    int       total_dots  = map_dots_remaining();
    int       hiscore     = hiscore_load(HISCORE_PATH);
    int       you_win     = 0;
    int       game_over   = 0;
    float     death_timer = 0.0f;
    int       paused      = 0;
    GameState state       = STATE_TITLE;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        audio_update();

        if (IsKeyPressed(KEY_F)) ToggleFullscreen();

        if (state == STATE_TITLE) {
            if (IsKeyPressed(KEY_ENTER)) {
                state = STATE_PLAYING;
            }
        } else {
            if (IsKeyPressed(KEY_M)) audio_toggle_music_mute();
            if (IsKeyPressed(KEY_N)) audio_toggle_sfx_mute();
            if (IsKeyPressed(KEY_LEFT_BRACKET))  audio_step_music_volume(-0.1f);
            if (IsKeyPressed(KEY_RIGHT_BRACKET)) audio_step_music_volume( 0.1f);
            if (IsKeyPressed(KEY_COMMA))  audio_step_sfx_volume(-0.1f);
            if (IsKeyPressed(KEY_PERIOD)) audio_step_sfx_volume( 0.1f);

            if (player.score > hiscore) hiscore = player.score;

            if (!you_win && !game_over && IsKeyPressed(KEY_P)) {
                paused = !paused;
                if (paused) audio_pause(); else audio_resume();
            }

            if (you_win || game_over) {
                if (IsKeyPressed(KEY_R)) {
                    hiscore_save(HISCORE_PATH, hiscore);
                    int reset_ok = 0;
                    if (you_win) {
                        int next_level = level + 1;
                        if (map_generate(next_level)) {
                            level = next_level;
                            player_respawn(&player);
                            reset_ok = 1;
                        }
                    } else {
                        if (map_generate(1)) {
                            level = 1;
                            player_init(&player);
                            reset_ok = 1;
                        }
                    }
                    if (reset_ok) {
                        int was_win = you_win;
                        ghosts_init_level(ghosts, level);
                        fruit_init(&fruit);
                        total_dots  = map_dots_remaining();
                        you_win     = 0;
                        game_over   = 0;
                        death_timer = 0.0f;
                        paused      = 0;
                        state       = was_win ? STATE_PLAYING : STATE_TITLE;
                    }
                }
            } else if (!paused) {
                game_update(&player, ghosts, &fruit, total_dots, dt,
                            &death_timer, &game_over, &you_win);
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);
            if (state == STATE_TITLE) {
                int tw = MeasureText("MUNCHER", 60);
                DrawText("MUNCHER",
                         (SCREEN_W - tw) / 2, SCREEN_H / 2 - 50, 60, YELLOW);
                tw = MeasureText("Press ENTER to start", 24);
                DrawText("Press ENTER to start",
                         (SCREEN_W - tw) / 2, SCREEN_H / 2 + 20, 24, WHITE);
            } else {
                DrawText("MUNCHER", 10, 4, 20, YELLOW);
                DrawText(TextFormat("SCORE: %d",  player.score), 130, 4, 20, WHITE);
                DrawText(TextFormat("LIVES: %d",  player.lives), 300, 4, 20, WHITE);
                DrawText(TextFormat("LEVEL: %d",  level),        430, 4, 20, WHITE);
                DrawText(TextFormat("BEST:  %d",  hiscore),      10, 32, 18, GRAY);
                map_draw(0, MAP_OFFSET_Y);
                if (!player.dead || (int)(death_timer * 6) % 2)
                    player_draw(&player, 0, MAP_OFFSET_Y);
                ghosts_draw(ghosts, 0, MAP_OFFSET_Y);
                fruit_draw(&fruit, 0, MAP_OFFSET_Y);
                if (you_win) {
                    int tw = MeasureText("LEVEL CLEAR!", 36);
                    DrawText("LEVEL CLEAR!",
                             (SCREEN_W - tw) / 2, SCREEN_H / 2 - 28, 36, YELLOW);
                    DrawText("Press R for next level",
                             SCREEN_W / 2 - 115, SCREEN_H / 2 + 18, 20, WHITE);
                }
                if (game_over) {
                    int tw = MeasureText("GAME OVER", 40);
                    DrawText("GAME OVER",
                             (SCREEN_W - tw) / 2, SCREEN_H / 2 - 20, 40, RED);
                    DrawText("Press R to restart",
                             SCREEN_W / 2 - 95, SCREEN_H / 2 + 30, 20, WHITE);
                }
                if (paused) {
                    int tw = MeasureText("PAUSED", 36);
                    DrawText("PAUSED",
                             (SCREEN_W - tw) / 2, SCREEN_H / 2 - 18, 36, WHITE);
                }
            }
        EndDrawing();
    }

    hiscore_save(HISCORE_PATH, hiscore);
    audio_close();
    CloseWindow();
    return 0;
}
