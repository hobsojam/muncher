#ifndef RAYLIB_H
#define RAYLIB_H

typedef struct { unsigned char r, g, b, a; } Color;
typedef struct { float x, y; }               Vector2;
typedef struct { int ctx; }                  Music;
typedef struct { int ctx; }                  Sound;

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

#define KEY_RIGHT 262
#define KEY_LEFT  263
#define KEY_DOWN  264
#define KEY_UP    265

static inline void DrawRectangle(int x, int y, int w, int h, Color c)
    { (void)x;(void)y;(void)w;(void)h;(void)c; }
static inline void DrawCircle(int cx, int cy, float r, Color c)
    { (void)cx;(void)cy;(void)r;(void)c; }
static inline void DrawCircleSector(Vector2 c, float r, float s, float e, int seg, Color col)
    { (void)c;(void)r;(void)s;(void)e;(void)seg;(void)col; }
static inline int IsKeyDown(int key) { (void)key; return 0; }
static inline void DrawText(const char *t, int x, int y, int s, Color c)
    { (void)t;(void)x;(void)y;(void)s;(void)c; }

static inline void  InitAudioDevice(void) {}
static inline void  CloseAudioDevice(void) {}
static inline Music LoadMusicStream(const char *f) { (void)f; return (Music){0}; }
static inline void  UnloadMusicStream(Music m) { (void)m; }
static inline void  PlayMusicStream(Music m) { (void)m; }
static inline void  UpdateMusicStream(Music m) { (void)m; }
static inline void  SetMusicVolume(Music m, float v) { (void)m; (void)v; }
static inline Sound LoadSound(const char *f) { (void)f; return (Sound){0}; }
static inline void  UnloadSound(Sound s) { (void)s; }
static inline void  PlaySound(Sound s) { (void)s; }
static inline void  SetSoundVolume(Sound s, float v) { (void)s; (void)v; }

#endif
