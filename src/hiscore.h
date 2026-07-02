#ifndef HISCORE_H
#define HISCORE_H

/* Save files are untrusted input (see CLAUDE_SECURITY.md): scores outside
   [0, HISCORE_MAX] are clamped on both load and save. */
#define HISCORE_MAX 9999999

int  hiscore_load(const char *path);
void hiscore_save(const char *path, int score);

#endif
