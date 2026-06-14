#ifndef PLAYER_INTERNAL_H
#define PLAYER_INTERNAL_H

enum {
    PLAYER_TUNNEL_ROW = 14,
};

int player_wrap_col(int col);
int player_can_enter(int col, int row);

#endif
