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
	struct character *character;
};

struct seen {
	int rows;
	int cols;
	int **tiles;
};

struct stats {
	int life;
	int stamina;
	int magic;
	int experience;
	int attack;
	int defense;
	int dodge;
	int power;
	int spirit;
	int avoid;
};

struct character {
	char *name;
	int level;
	int money;
	int major_points;
	int minor_points;
	struct stats cur_stats;
	struct stats max_stats;
	int inventory[8];
	int gear[3];
	int skills[3];
};

enum dirs { NORTH, EAST, SOUTH, WEST };

/* Function prototypes */
extern void		init_seen(struct seen *cur_seen, int rows, int cols);
extern void		kill_seen(struct seen *cur_seen);
extern void		update_seen(struct user *cur_user);
extern SDL_bool		move_player(struct game *cur_game, struct user *cur_user, int dir);
extern SDL_bool		strafe_player(struct game *cur_game, struct user *cur_user, int dir);
extern void		turn_player(struct user *cur_user, int turn);
extern void		change_level(struct game *cur_game, struct map *cur_map, struct user *cur_user);

#endif
