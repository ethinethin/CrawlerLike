#ifndef USER_H
#define USER_H

#include "main.h"

struct user {
	int map;
	int row;
	int col;
	int facing;
	int num_seen;
	struct seen *seen;
};

struct seen {
	int rows;
	int cols;
	int **tiles;
};

enum dirs { NORTH, EAST, SOUTH, WEST };

/* Function prototypes */
extern void		init_seen(struct seen *cur_seen, int rows, int cols);
extern void		kill_seen(struct seen *cur_seen);
extern void		update_seen(struct user *cur_user);
extern SDL_bool		move_player(struct map *cur_map, struct user *cur_user, int move);
extern void		turn_player(struct user *cur_user, int turn);
extern void		change_level(struct game *cur_game, struct map *cur_map, struct user *cur_user);

#endif
