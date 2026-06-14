#ifndef MAP_INTERNAL_H
#define MAP_INTERNAL_H

#include "map.h"

enum {
    MAP_GENERATION_ATTEMPTS = 20,
};

typedef int (*MapValidator)(void);

int map_all_dots_reachable(void);
int map_generate_with_validator(int level, MapValidator validator);

#endif
