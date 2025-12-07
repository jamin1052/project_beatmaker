#ifndef MAP_H
#define MAP_H

#include "defs.h"

#define ROOM_ROWS 3
#define ROOM_COLS 4

extern int current_room_row;
extern int current_room_col;

extern int g_map_data[MAP_HEIGHT][MAP_WIDTH];

void LoadRoom(int row, int col);
void ChangeRoom(int drow, int dcol);

#endif
