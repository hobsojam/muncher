#ifndef MAP_H
#define MAP_H

#define MAP_COLS  28
#define MAP_ROWS  31
#define TILE_SIZE 20

typedef enum {
    TILE_WALL  = 0,
    TILE_DOT   = 1,
    TILE_POWER = 2,
    TILE_EMPTY = 3,
    TILE_DOOR  = 4,
} TileType;

extern TileType map[MAP_ROWS][MAP_COLS];

int  map_generate(int level);
void map_init(void);
void map_draw(int offset_x, int offset_y);
int  map_dots_remaining(void);

#endif
