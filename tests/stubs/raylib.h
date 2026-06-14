#ifndef RAYLIB_H
#define RAYLIB_H

#include <stdio.h>
#include <stdarg.h>

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;
typedef struct {
    float x;
    float y;
} Vector2;
typedef struct { int ctx; }                  Music;
typedef struct { int ctx; }                  Sound;

static int raylib_stub_music_load_success = 1;
static int raylib_stub_sound_load_success = 1;
static int raylib_stub_invalid_music_operations = 0;
static int raylib_stub_invalid_sound_operations = 0;

static inline void RaylibStubResetAudio(void) {
    raylib_stub_music_load_success = 1;
    raylib_stub_sound_load_success = 1;
    raylib_stub_invalid_music_operations = 0;
    raylib_stub_invalid_sound_operations = 0;
}

#define RED      (Color){230, 41,  55,  255}
#define GREEN    (Color){0,   228, 48,  255}
#define BLUE     (Color){0,   121, 241, 255}
#define WHITE    (Color){255, 255, 255, 255}
#define BLACK    (Color){0,   0,   0,   255}
#define YELLOW   (Color){253, 249, 0,   255}
#define PINK     (Color){255, 109, 194, 255}
#define ORANGE   (Color){255, 161, 0,   255}
#define SKYBLUE  (Color){102, 191, 255, 255}
#define DARKBLUE (Color){0,   82,  172, 255}

#define KEY_RIGHT  262
#define KEY_LEFT   263
#define KEY_DOWN   264
#define KEY_UP     265
#define KEY_ENTER  257
#define KEY_P       80
#define KEY_F11    300

#define FLAG_WINDOW_RESIZABLE 0x00000004

static inline void SetWindowState(unsigned int flags) { (void)flags; }
static inline int  IsWindowMaximized(void)            { return 0; }
static inline void RestoreWindow(void)                {}
static inline void ToggleFullscreen(void)             {}

static inline void DrawRectangle(int x, int y, int w, int h, Color c)
    { (void)x;(void)y;(void)w;(void)h;(void)c; }
static inline void DrawCircle(int cx, int cy, float r, Color c)
    { (void)cx;(void)cy;(void)r;(void)c; }
static inline void DrawCircleSector(Vector2 c, float r, float s, float e, int seg, Color col)
    { (void)c;(void)r;(void)s;(void)e;(void)seg;(void)col; }
static inline int IsKeyDown(int key) { (void)key; return 0; }
static inline void DrawText(const char *t, int x, int y, int s, Color c)
    { (void)t;(void)x;(void)y;(void)s;(void)c; }
static inline const char *TextFormat(const char *fmt, ...) {
    static char buf[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return buf;
}

static inline void  InitAudioDevice(void) {}
static inline void  CloseAudioDevice(void) {}
static inline Music LoadMusicStream(const char *f)
    { (void)f; return (Music){raylib_stub_music_load_success ? 1 : 0}; }
static inline int IsMusicValid(Music m) { return m.ctx != 0; }
static inline void  UnloadMusicStream(Music m)
    { if (!IsMusicValid(m)) raylib_stub_invalid_music_operations++; }
static inline void  PlayMusicStream(Music m)
    { if (!IsMusicValid(m)) raylib_stub_invalid_music_operations++; }
static inline void  PauseMusicStream(Music m)
    { if (!IsMusicValid(m)) raylib_stub_invalid_music_operations++; }
static inline void  ResumeMusicStream(Music m)
    { if (!IsMusicValid(m)) raylib_stub_invalid_music_operations++; }
static inline void  UpdateMusicStream(Music m)
    { if (!IsMusicValid(m)) raylib_stub_invalid_music_operations++; }
static inline void  SetMusicVolume(Music m, float v)
    { (void)v; if (!IsMusicValid(m)) raylib_stub_invalid_music_operations++; }
static inline Sound LoadSound(const char *f)
    { (void)f; return (Sound){raylib_stub_sound_load_success ? 1 : 0}; }
static inline int IsSoundValid(Sound s) { return s.ctx != 0; }
static inline void  UnloadSound(Sound s)
    { if (!IsSoundValid(s)) raylib_stub_invalid_sound_operations++; }
static inline void  PlaySound(Sound s)
    { if (!IsSoundValid(s)) raylib_stub_invalid_sound_operations++; }
static inline void  SetSoundVolume(Sound s, float v)
    { (void)v; if (!IsSoundValid(s)) raylib_stub_invalid_sound_operations++; }

#endif
