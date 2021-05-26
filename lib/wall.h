#ifndef WALL_H
#define WALL_H

#include "main.h"
#include "user.h"

extern void	draw_map(struct game *cur_game, struct user *cur_user, int x, int y);
extern void	draw_view(struct game *cur_game, struct user *cur_user);

#endif
