#include "hiscore.h"
#include <stdio.h>

int hiscore_load(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    int score = 0;
    if (fread(&score, sizeof(int), 1, f) != 1) score = 0;
    fclose(f);
    return score > 0 ? score : 0;
}

void hiscore_save(const char *path, int score) {
    if (score <= 0) return;
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fwrite(&score, sizeof(int), 1, f);
    fclose(f);
}
