#ifndef CHAR_H
#define CHAR_H

#include <SDL2/SDL.h>
#include "main.h"
#include "user.h"

extern void	kill_char(struct user *cur_user);
extern void	init_char(struct game *cur_game, struct user *cur_user);
extern void	char_screen(struct game *cur_game, struct user *cur_user);
extern void	zero_stats(struct stats *cur_stats);
extern void	copy_stats(struct stats *src, struct stats *dest);
extern void	update_stats(struct user *cur_user);

#endif
