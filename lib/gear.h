#ifndef GEAR_H
#define GEAR_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include "main.h"
#include "user.h"

extern SDL_bool		 drag_mouse(struct game *cur_game, struct user *cur_user, int x, int y);
extern void		 gear_mouseover(struct game *cur_game, struct user *cur_user, int x, int y);
extern void		 clear_info(struct game *cur_game);
extern void		 init_gear(void);
extern void		 kill_gear(void);
extern int		 gear_sprite(int id);
extern struct stats	*gear_stats(int id);
extern int		 create_gear(int level);
extern void		 dump_gear(FILE *fp);
extern void		 undump_gear(FILE *fp);

enum rarity { RAR_TRASH = 0, RAR_COMMON, RAR_UNCOMMON, RAR_RARE, RAR_RAREPLUS, RAR_UNIQUE };
enum gear_type { GEAR_WEAPON, GEAR_ARMOR, GEAR_ACCESSORY, GEAR_SKILL, GEAR_ITEM, GEAR_ANY, GEAR_TRASH, GEAR_NULL };

#endif
