#include "hiscore.h"
#include <stdio.h>

static int clamp_score(int score) {
    if (score < 0) return 0;
    if (score > HISCORE_MAX) return HISCORE_MAX;
    return score;
}

int hiscore_load(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    int score = 0;
    if (fread(&score, sizeof(int), 1, f) != 1) score = 0;
    fclose(f);
    return clamp_score(score);
}

void hiscore_save(const char *path, int score) {
    score = clamp_score(score);
    if (score == 0) return;
    FILE *f = fopen(path, "wb");
    if (!f) return;
    /* Taint analysis flags this flow (save file -> fread -> fwrite). Accepted:
       the tainted value is the data being written, not a size/path/format, and
       it is clamped to [0, HISCORE_MAX] at the trust boundary in hiscore_load. */
    fwrite(&score, sizeof(int), 1, f);
    fclose(f);
}
