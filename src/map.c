#include "map.h"
#include "map_internal.h"
#include "raylib.h"

TileType map[MAP_ROWS][MAP_COLS];

/* ------------------------------------------------------------------ */
/* Procedural generation                                                */
/* Cell (ci, cj) occupies tile (2ci+1, 2cj+1).                        */
/* Passage between (ci,cj) and (ni,nj) is at tile (ci+ni+1, cj+nj+1).*/
/* ------------------------------------------------------------------ */

#define CELL_ROWS 15
#define CELL_COLS 13

static int   s_vis[CELL_ROWS][CELL_COLS];
static Color s_wall_color;

static const Color WALL_PALETTE[] = {
    {33,  33,  222, 255},
    {0,   160, 0,   255},
    {180, 0,   180, 255},
    {210, 80,  0,   255},
    {0,   180, 180, 255},
};
#define PALETTE_SIZE 5

static unsigned int lcg(unsigned int *s) {
    *s = *s * 1664525u + 1013904223u;
    return *s;
}

/* Ghost zone: tile (2ci+1, 2cj+1) falls inside ghost house walls. */
static int ghost_zone(int ci, int cj) {
    int tr = 2*ci + 1;
    int tc = 2*cj + 1;
    return tr >= 12 && tr <= 16 && tc >= 10 && tc <= 17;
}

static const int DROW[4] = {-1, 1, 0, 0};
static const int DCOL[4] = { 0, 0,-1, 1};

static void carve(int ci, int cj, unsigned int *rng) {
    s_vis[ci][cj] = 1;
    map[2*ci+1][2*cj+1] = TILE_EMPTY;

    int ord[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int j = (int)(lcg(rng) % (unsigned int)(i + 1));
        int t = ord[i];
        ord[i] = ord[j];
        ord[j] = t;
    }
    for (int k = 0; k < 4; k++) {
        int ni = ci + DROW[ord[k]];
        int nj = cj + DCOL[ord[k]];
        if (ni < 0 || ni >= CELL_ROWS || nj < 0 || nj >= CELL_COLS) continue;
        if (s_vis[ni][nj]) continue;
        map[ci + ni + 1][cj + nj + 1] = TILE_EMPTY;
        carve(ni, nj, rng);
    }
}

/* For each dead-end cell, open one wall to a carved neighbour. */
static void braid_maze(unsigned int *rng) {
    for (int ci = 0; ci < CELL_ROWS; ci++) {
        for (int cj = 0; cj < CELL_COLS; cj++) {
            if (ghost_zone(ci, cj)) continue;
            int tr = 2*ci + 1;
            int tc = 2*cj + 1;

            int open = 0;
            int wpr[4];
            int wpc[4];
            int nw = 0;

            for (int k = 0; k < 4; k++) {
                int pr = tr + DROW[k];
                int pc = tc + DCOL[k];
                if (pr < 0 || pr >= MAP_ROWS || pc < 0 || pc >= MAP_COLS) continue;
                if (map[pr][pc] == TILE_EMPTY) {
                    open++;
                } else if (map[pr][pc] == TILE_WALL) {
                    int nr = pr + DROW[k];
                    int nc = pc + DCOL[k];
                    if (nr < 0 || nr >= MAP_ROWS || nc < 0 || nc >= MAP_COLS) continue;
                    if (map[nr][nc] != TILE_EMPTY) continue;
                    if (ghost_zone((nr - 1) / 2, (nc - 1) / 2)) continue;
                    if (nw < 4) { wpr[nw] = pr; wpc[nw] = pc; nw++; }
                }
            }

            if (open == 1 && nw > 0) {
                int pick = (int)(lcg(rng) % (unsigned int)nw);
                map[wpr[pick]][wpc[pick]] = TILE_EMPTY;
            }
        }
    }
}

static void stamp_ghost_house(void) {
    for (int c = 10; c <= 12; c++) map[12][c] = TILE_WALL;
    map[12][13] = TILE_DOOR;
    map[12][14] = TILE_DOOR;
    for (int c = 15; c <= 17; c++) map[12][c] = TILE_WALL;
    for (int r = 13; r <= 15; r++) {
        map[r][10] = TILE_WALL;
        for (int c = 11; c <= 16; c++) map[r][c] = TILE_EMPTY;
        map[r][17] = TILE_WALL;
    }
    for (int c = 10; c <= 17; c++) map[16][c] = TILE_WALL;
}

static void stamp_tunnel(void) {
    for (int c = 0;  c <= 5;  c++) map[14][c] = TILE_EMPTY;
    for (int c = 6;  c <= 9;  c++) map[14][c] = TILE_DOT;
    for (int c = 18; c <= 21; c++) map[14][c] = TILE_DOT;
    for (int c = 22; c <= 27; c++) map[14][c] = TILE_EMPTY;
}

static void place_power_pellets(unsigned int *rng) {
    static const int QUADS[4][4] = {
        { 1, 11,  1, 13},
        { 1, 11, 14, 26},
        {18, 29,  1, 13},
        {18, 29, 14, 26},
    };
    for (int q = 0; q < 4; q++) {
        for (int attempt = 0; attempt < 20; attempt++) {
            int r = QUADS[q][0] + (int)(lcg(rng) % (unsigned int)(QUADS[q][1] - QUADS[q][0] + 1));
            int c = QUADS[q][2] + (int)(lcg(rng) % (unsigned int)(QUADS[q][3] - QUADS[q][2] + 1));
            if (map[r][c] == TILE_DOT) {
                map[r][c] = TILE_POWER;
                break;
            }
        }
    }
}

int map_all_dots_reachable(void) {
    static int seen[MAP_ROWS][MAP_COLS];
    static int qr[MAP_ROWS * MAP_COLS];
    static int qc[MAP_ROWS * MAP_COLS];
    static const int DR[4] = {-1, 1, 0, 0};
    static const int DC[4] = { 0, 0,-1, 1};

    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            seen[r][c] = 0;

    int head = 0;
    int tail = 0;
    seen[29][14] = 1;
    qr[tail] = 29; qc[tail++] = 14;

    while (head < tail) {
        int r = qr[head];
        int c = qc[head++];
        for (int d = 0; d < 4; d++) {
            int nr = r + DR[d];
            int nc = c + DC[d];
            if (nr < 0 || nr >= MAP_ROWS) continue;
            if (nc < 0) nc = MAP_COLS - 1;
            if (nc >= MAP_COLS) nc = 0;
            if (seen[nr][nc]) continue;
            TileType t = map[nr][nc];
            if (t == TILE_WALL || t == TILE_DOOR) continue;
            seen[nr][nc] = 1;
            qr[tail] = nr; qc[tail++] = nc;
        }
    }

    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            if ((map[r][c] == TILE_DOT || map[r][c] == TILE_POWER) && !seen[r][c])
                return 0;
    return 1;
}

int map_generate_with_validator(int level, MapValidator validator) {
    TileType previous_map[MAP_ROWS][MAP_COLS];
    Color previous_wall_color = s_wall_color;
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            previous_map[r][c] = map[r][c];

    s_wall_color = WALL_PALETTE[((level - 1) / 5) % PALETTE_SIZE];
    unsigned int rng = (unsigned int)level * 2654435761u;

    for (int attempt = 0; attempt < MAP_GENERATION_ATTEMPTS; attempt++) {
        for (int r = 0; r < MAP_ROWS; r++)
            for (int c = 0; c < MAP_COLS; c++)
                map[r][c] = TILE_WALL;

        for (int i = 0; i < CELL_ROWS; i++)
            for (int j = 0; j < CELL_COLS; j++)
                s_vis[i][j] = ghost_zone(i, j);

        carve(14, 6, &rng);

        /* Ghost spawn corridor (row 11) and player start must always be passable. */
        for (int c = 9; c <= 19; c++) map[11][c] = TILE_EMPTY;
        map[29][14] = TILE_EMPTY;

        braid_maze(&rng);

        for (int r = 1; r < MAP_ROWS - 1; r++)
            for (int c = 1; c < MAP_COLS - 1; c++)
                if (map[r][c] == TILE_EMPTY)
                    map[r][c] = TILE_DOT;

        stamp_ghost_house();
        stamp_tunnel();
        place_power_pellets(&rng);

        if (validator()) return 1;
        rng = rng * 1664525u + 1013904223u;
    }

    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            map[r][c] = previous_map[r][c];
    s_wall_color = previous_wall_color;
    return 0;
}

int map_generate(int level) {
    return map_generate_with_validator(level, map_all_dots_reachable);
}

void map_init(void) { (void)map_generate(1); }

/* ------------------------------------------------------------------ */

int map_dots_remaining(void) {
    int count = 0;
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            if (map[r][c] == TILE_DOT || map[r][c] == TILE_POWER)
                count++;
    return count;
}

void map_draw(int offset_x, int offset_y) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int x    = offset_x + col * TILE_SIZE;
            int y    = offset_y + row * TILE_SIZE;
            int half = TILE_SIZE / 2;

            switch (map[row][col]) {
                case TILE_WALL:
                    DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, s_wall_color);
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
