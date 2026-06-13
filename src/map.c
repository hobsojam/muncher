#include "map.h"
#include "raylib.h"

TileType map[MAP_ROWS][MAP_COLS];

// '#' wall  '.' dot  'o' power pellet  '-' ghost door  ' ' empty
static const char LAYOUT[MAP_ROWS][MAP_COLS + 1] = {
    "############################",   // 0
    "#............##............#",   // 1
    "#.####.#####.##.#####.####.#",   // 2
    "#o####.#####.##.#####.####o#",   // 3
    "#.####.#####.##.#####.####.#",   // 4
    "#..........................#",   // 5
    "#.####.##.########.##.####.#",   // 6
    "#.####.##.########.##.####.#",   // 7
    "#......##....##....##......#",   // 8
    "######.#####.##.#####.######",   // 9
    "     #.#####.##.#####.#     ",   // 10
    "     #.##..........##.#     ",   // 11
    "     #.##.###--###.##.#     ",   // 12
    "######.##.#      #.##.######",   // 13
    "      ....#      #....      ",   // 14 (tunnel row)
    "######.##.#      #.##.######",   // 15
    "     #.##.########.##.#     ",   // 16
    "     #.##..........##.#     ",   // 17
    "     #.##.########.##.#     ",   // 18
    "######.##.########.##.######",   // 19
    "#............##............#",   // 20
    "#.####.#####.##.#####.####.#",   // 21
    "#o..##................##..o#",   // 22
    "###.##.##.########.##.##.###",   // 23
    "###.##.##.########.##.##.###",   // 24
    "#......##....##....##......#",   // 25
    "#.##########.##.##########.#",   // 26
    "#.##########.##.##########.#",   // 27
    "#..........................#",   // 28
    "#..........................#",   // 29
    "############################",   // 30
};

void map_init(void) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            switch (LAYOUT[row][col]) {
                case '#': map[row][col] = TILE_WALL;  break;
                case '.': map[row][col] = TILE_DOT;   break;
                case 'o': map[row][col] = TILE_POWER; break;
                case '-': map[row][col] = TILE_DOOR;  break;
                default:  map[row][col] = TILE_EMPTY; break;
            }
        }
    }
}

void map_draw(int offset_x, int offset_y) {
    static const Color WALL_COLOR = {33, 33, 222, 255};

    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int x = offset_x + col * TILE_SIZE;
            int y = offset_y + row * TILE_SIZE;
            int half = TILE_SIZE / 2;

            switch (map[row][col]) {
                case TILE_WALL:
                    DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, WALL_COLOR);
                    break;
                case TILE_DOT:
                    DrawCircle(x + half, y + half, 2, WHITE);
                    break;
                case TILE_POWER:
                    DrawCircle(x + half, y + half, 5, WHITE);
                    break;
                case TILE_DOOR:
                    DrawRectangle(x, y + half - 1, TILE_SIZE, 2, PINK);
                    break;
                case TILE_EMPTY:
                    break;
            }
        }
    }
}
