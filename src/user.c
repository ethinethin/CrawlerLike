#include <SDL2/SDL.h>
#include "draw.h"
#include "font.h"
#include "home.h"
#include "main.h"
#include "rand.h"
#include "user.h"

/* Function prototypes */
static void	pass_time(struct game *cur_game);

void
init_seen(struct seen *cur_seen, int rows, int cols)
{
	int i, j;
	
	/* Set up seen dimensions */
	cur_seen->rows = rows;
	cur_seen->cols = cols;
	/* Allocate memory and set to 0 */
	cur_seen->tiles = malloc(sizeof(*cur_seen->tiles) * rows);
	for (i = 0; i < rows; i++) {
		*(cur_seen->tiles + i) = malloc(sizeof(**cur_seen->tiles) * cols);
		for (j = 0; j < cols; j++) {
			*(*(cur_seen->tiles + i) + j) = 0;
		}
	}
}

void
kill_seen(struct seen *cur_seen)
{
	int i;

	/* Free memory */
	for (i = 0; i < cur_seen->rows; i++) {
		free(*(cur_seen->tiles + i));
	}
	free(cur_seen->tiles);
}

void
update_seen(struct user *cur_user)
{
	*(*(cur_user->seen[cur_user->map].tiles + cur_user->row) + cur_user->col) = 1;
}

SDL_bool
move_player(struct game *cur_game, struct user *cur_user, int move)
{
	int row_d, col_d;
	struct map *cur_map;
		
	/* Set the direction to be changed based on facing direction */
	row_d = 0;
	col_d = 0;
	if (cur_user->facing == NORTH) row_d = -1;
	if (cur_user->facing == EAST) col_d = 1;
	if (cur_user->facing == SOUTH) row_d = 1;
	if (cur_user->facing == WEST) col_d = -1;
	/* Point to the right map */
	cur_map =  cur_game->maps + cur_user->map;
	/* Multiply by move, which is negative 1 if the player is backing up */
	row_d *= move;
	col_d *= move;
	/* See if the move is okay */
	if (*(*(cur_map->tiles + cur_user->row + row_d) + cur_user->col + col_d) == ROOM ||
	    *(*(cur_map->tiles + cur_user->row + row_d) + cur_user->col + col_d) == DOOR) {
		cur_user->row += row_d * 2;
		cur_user->col += col_d * 2;
		pass_time(cur_game);
		return SDL_TRUE;
	}
	return SDL_FALSE;
}

void
turn_player(struct user *cur_user, int turn)
{
	cur_user->facing += turn;
	if (cur_user->facing < NORTH) cur_user->facing = WEST;
	if (cur_user->facing > WEST) cur_user->facing = NORTH;
}

void
change_level(struct game *cur_game, struct map *cur_map, struct user *cur_user)
{
	int cur_level;
	
	cur_level = cur_user->map;
	if (*(*(cur_map->tiles + cur_user->row) + cur_user->col) == START && cur_level != 0) {
		cur_user->map -= 1;
	} else if (*(*(cur_map->tiles + cur_user->row) + cur_user->col) == END && cur_level < cur_game->num_maps - 1) {
		cur_user->map += 1;
	}
	if (cur_level != cur_user->map) pass_time(cur_game);
}

static void
pass_time(struct game *cur_game)
{
	cur_game->minute += 1;
	if (cur_game->minute == 144) {
		cur_game->day += 1;
		cur_game->minute = 0;
	}
}
