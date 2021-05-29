#ifndef HOME_H
#define HOME_H

#include "main.h"
#include "user.h"

extern void	title(struct game *cur_game, struct user *cur_user);

enum game_states { UNLOADED, LOADED };

#endif
