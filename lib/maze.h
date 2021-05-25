#ifndef MAZE_H
#define MAZE_H

#include "main.h"
#include "user.h"

/* Function prototypes */
extern void	init_map(struct map *cur_map, int rows, int cols);
extern void	kill_map(struct map *cur_map);
extern void	populate_map(struct map *cur_map, int row_start, int col_start);
extern void	draw_map(struct game *cur_game, struct user *cur_user, int x, int y);
extern void	draw_view(struct game *cur_game, struct user *cur_user);

#endif
