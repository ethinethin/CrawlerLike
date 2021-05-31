#ifndef SAVE_H
#define SAVE_H

#include <SDL2/SDL.h>
#include "main.h"
#include "user.h"

struct savefile_info {
	SDL_bool exists;
	char line1[80];
	char line2[80];
	char line3[80];
};

extern void	save_opts(struct game *cur_game);
extern void	load_opts(struct game *cur_game);
extern void	load_info(struct savefile_info *save);
extern void	save_all(struct game *cur_game, struct user *cur_user);
extern void	load_all(struct game *cur_game, struct user *cur_user);

#endif
