#ifndef CHAR_H
#define CHAR_H

#include <SDL2/SDL.h>
#include "main.h"
#include "user.h"

extern void		kill_char(struct user *cur_user);
extern void		init_char(struct game *cur_game, struct user *cur_user);
extern void		char_screen(struct game *cur_game, struct user *cur_user, SDL_bool ingame);

#endif
