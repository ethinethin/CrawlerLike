#ifndef GEAR_H
#define GEAR_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include "main.h"
#include "user.h"

extern SDL_bool		 drag_mouse(struct game *cur_game, struct user *cur_user, int x, int y);
extern void		 init_gear(void);
extern void		 kill_gear(void);
extern void		 add_gear(int id, int sprite, int type, int level, int value, struct stats *mod);
extern void		 del_gear(int id);
extern int		 gear_sprite(int id);
extern struct stats	*gear_stats(int id);
extern void		 dump_gear(FILE *fp);
extern void		 undump_gear(FILE *fp);

#endif
