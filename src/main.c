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
#define GAME_W            (MAP_COLS * TILE_SIZE)
#define GAME_H            (MAP_ROWS * TILE_SIZE + 60)
#define MAP_OFFSET_Y      60
#define DEATH_FREEZE_SECS 1.5f
#define READY_SECS        3.0f

typedef enum { STATE_TITLE = 0, STATE_PLAYING } GameState;

static void game_update(Player *p, Ghost ghosts[], Fruit *fruit, int total_dots,
                        float dt, float *death_timer, float *ready_timer,
                        int *game_over, int *you_win) {
    if (*ready_timer > 0.0f) {
        *ready_timer -= dt;
        if (*ready_timer < 0.0f) *ready_timer = 0.0f;
        return;
    }
    if (*death_timer > 0.0f) {
        *death_timer -= dt;
        if (*death_timer <= 0.0f) {
            *death_timer = 0.0f;
            handle_ghost_collision(p, ghosts, game_over);
            if (!*game_over) *ready_timer = READY_SECS;
        }
        return;
    }
    player_update(p, dt);
    if (p->ate_power) ghosts_frighten(ghosts);
    ghosts_update(ghosts, p, dt);
    handle_collision(p, ghosts);
    fruit_update(fruit, p, map_dots_remaining(), total_dots, dt);
    player_check_extra_life(p);
    if (p->dead) {
        *death_timer = DEATH_FREEZE_SECS;
        audio_play_death();
    }
    if (map_dots_remaining() == 0) *you_win = 1;
}

static void draw_game_to_screen(RenderTexture2D target) {
    float sw    = (float)GetScreenWidth();
    float sh    = (float)GetScreenHeight();
    float scale = sw / GAME_W;
    if (sh / GAME_H < scale) scale = sh / GAME_H;
    Rectangle src = { 0.0f, 0.0f, (float)GAME_W, -(float)GAME_H };
    Rectangle dst = {
        (sw - GAME_W * scale) / 2.0f,
        (sh - GAME_H * scale) / 2.0f,
        (float)GAME_W * scale,
        (float)GAME_H * scale
    };
    DrawTexturePro(target.texture, src, dst, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

int main(void) {
    InitWindow(GAME_W, GAME_H, "Muncher");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    audio_init();
    srand((unsigned)time(NULL));

    RenderTexture2D target = LoadRenderTexture(GAME_W, GAME_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    int level = 1;
    if (!map_generate(level)) {
        UnloadRenderTexture(target);
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
    float     ready_timer = READY_SECS;
    int       paused      = 0;
    GameState state       = STATE_TITLE;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        audio_update();

        if (IsKeyPressed(KEY_F11) || IsWindowMaximized()) {
            if (IsWindowMaximized()) RestoreWindow();
            ToggleFullscreen();
        }

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
                        ready_timer = READY_SECS;
                        paused      = 0;
                        state       = was_win ? STATE_PLAYING : STATE_TITLE;
                    }
                }
            } else if (!paused) {
                game_update(&player, ghosts, &fruit, total_dots, dt,
                            &death_timer, &ready_timer, &game_over, &you_win);
            }
        }

        BeginTextureMode(target);
            ClearBackground(BLACK);
            if (state == STATE_TITLE) {
                int tw = MeasureText("MUNCHER", 60);
                DrawText("MUNCHER",
                         (GAME_W - tw) / 2, GAME_H / 2 - 50, 60, YELLOW);
                tw = MeasureText("Press ENTER to start", 24);
                DrawText("Press ENTER to start",
                         (GAME_W - tw) / 2, GAME_H / 2 + 20, 24, WHITE);
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
                             (GAME_W - tw) / 2, GAME_H / 2 - 28, 36, YELLOW);
                    DrawText("Press R for next level",
                             GAME_W / 2 - 115, GAME_H / 2 + 18, 20, WHITE);
                }
                if (game_over) {
                    int tw = MeasureText("GAME OVER", 40);
                    DrawText("GAME OVER",
                             (GAME_W - tw) / 2, GAME_H / 2 - 20, 40, RED);
                    DrawText("Press R to restart",
                             GAME_W / 2 - 95, GAME_H / 2 + 30, 20, WHITE);
                }
                if (paused) {
                    int tw = MeasureText("PAUSED", 36);
                    DrawText("PAUSED",
                             (GAME_W - tw) / 2, GAME_H / 2 - 18, 36, WHITE);
                }
                if (ready_timer > 0.0f && !you_win && !game_over) {
                    const char *num = TextFormat("%d", (int)ready_timer + 1);
                    int tw = MeasureText(num, 72);
                    DrawText(num, (GAME_W - tw) / 2, GAME_H / 2 - 36, 72, YELLOW);
                }
            }
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            draw_game_to_screen(target);
        EndDrawing();
    }

    hiscore_save(HISCORE_PATH, hiscore);
    UnloadRenderTexture(target);
    audio_close();
    CloseWindow();
    return 0;
}
