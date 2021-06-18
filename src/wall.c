#include <SDL2/SDL.h>
#include "draw.h"
#include "main.h"
#include "user.h"
#include "wall.h"

/* I've cleaned these functions up a bit. Setting the wall coordinates is especially a
 * good bit cleaner. Drawing still needs some work but it's much easier to read now */

/* Function prototypes */
static void	get_coords(struct user *cur_user, struct coords *cur_wall, int forward, int side);
static void	set_walls(struct user *cur_user, struct coords walls[25]);
static void	set_junk(struct user *cur_user, struct coords junk[9]);
static void	check_walls(struct game *cur_game, struct user *cur_user, struct coords walls[25]);
static void	check_junk(struct game *cur_game, struct user *cur_user, struct coords junk[9]);
static void	draw_wall(struct game *cur_game, struct user *cur_user, SDL_Texture **walls, struct coords *wall_coords, int wall_num, int sprite_num);
static void	draw_junk(struct game *cur_game, SDL_Texture **junk, int junk_num);


void
draw_map(struct game *cur_game, struct user *cur_user, int x, int y)
{
	int i, j;
	int x_coord, y_coord;
	int map_num;

	/* Draw map */
	map_num =  cur_user->map;
	/* Draw a filled in blue rectangle with a white border */
	draw_rect(cur_game, x + 1, y + 1, 319, 319, SDL_TRUE, "darkblue");
	draw_rect(cur_game, x, y, 321, 321, SDL_FALSE, "white");
	/* Draw all walls adjacent to map spaces */
	for (i = 1; i < cur_game->maps[map_num].rows - 1; i += 2) {
		for (j = 1; j < cur_game->maps[map_num].cols - 1; j += 2) {
			/* This flattens the coordinates to account for even-numbered wall grids */
			x_coord = j/2; y_coord = i/2;
			/* Only draw if the space has been seen */
			if (*(*(cur_user->seen[cur_user->map].tiles + i) + j) == 1) {
				draw_rect(cur_game, x + x_coord * 20, y + y_coord * 20, 20, 20, SDL_TRUE, "black");
				/* North Wall/Door */
				if (cur_game->maps[map_num].tiles[i - 1][j] == WALL) {
					draw_line(cur_game, x + x_coord * 20, y + y_coord * 20, x + (x_coord + 1) * 20, y + y_coord * 20, "white");
				} else if (cur_game->maps[map_num].tiles[i - 1][j] == DOOR) {
					draw_rect(cur_game, x + x_coord * 20 + 1, y + y_coord * 20 - 1, 19, 3, SDL_TRUE, "white");
				}
				if (cur_game->maps[map_num].tiles[i][j + 1] == WALL) {
					draw_line(cur_game, x + (x_coord + 1) * 20, y + y_coord * 20, x + (x_coord + 1) * 20, y + (y_coord + 1) * 20, "white");
				} else if (cur_game->maps[map_num].tiles[i][j + 1] == DOOR) {
					draw_rect(cur_game, x + x_coord * 20 + 19, y + y_coord * 20 + 1, 3, 19, SDL_TRUE, "white");
				}
				if (cur_game->maps[map_num].tiles[i + 1][j] == WALL) {
					draw_line(cur_game, x + x_coord * 20, y + (y_coord + 1) * 20, x + (x_coord + 1) * 20, y + (y_coord + 1) * 20, "white");
				} else if (cur_game->maps[map_num].tiles[i + 1][j] == DOOR) {
					draw_rect(cur_game, x + x_coord * 20 + 1, y + y_coord * 20 + 19, 19, 3, SDL_TRUE, "white");
				}
				if (cur_game->maps[map_num].tiles[i][j - 1] == WALL) {
					draw_line(cur_game, x + x_coord * 20, y + y_coord * 20, x + x_coord * 20, y + (y_coord + 1) * 20, "white");
				} else if (cur_game->maps[map_num].tiles[i][j - 1] == DOOR) {
					draw_rect(cur_game, x + x_coord * 20 - 1, y + y_coord * 20 + 1, 3, 19, SDL_TRUE, "white");
				}
				if (cur_game->maps[map_num].tiles[i][j] == START) {
					draw_sprites(cur_game, cur_game->sprites.arrows, 4, x + x_coord * 20 + 2, y + y_coord * 20 + 2, 16, 16, 255, SDL_FALSE);
				} else if (cur_game->maps[map_num].tiles[i][j] == END) {
					draw_sprites(cur_game, cur_game->sprites.arrows, 5, x + x_coord * 20 + 2, y + y_coord * 20 + 2, 16, 16, 255, SDL_FALSE);
				}
			} else {
				/* Draw a small white square if the room has not been seen */
				draw_rect(cur_game, x + x_coord * 20 + 9, y + y_coord * 20 + 9, 2, 2, SDL_TRUE, "white");
			}
		}
	}
	/* Draw the player arrow */
	x_coord = cur_user->col/2; y_coord = cur_user->row/2;
	draw_sprites(cur_game, cur_game->sprites.arrows, cur_user->facing, x + x_coord * 20 + 2, y + y_coord * 20 + 2, 16, 16, 255, SDL_FALSE);
}

static void
get_coords(struct user *cur_user, struct coords *cur_wall, int forward, int side)
{
	/* Depending on facing, forward and sides are different parameters */
	if (cur_user->facing == NORTH) {
		cur_wall->row = cur_user->row - forward;
		cur_wall->col = cur_user->col + side;
	} else if (cur_user->facing == EAST) {
		cur_wall->col = cur_user->col + forward;
		cur_wall->row = cur_user->row + side;
	} else if (cur_user->facing == SOUTH) {
		cur_wall->row = cur_user->row + forward;
		cur_wall->col = cur_user->col - side;
	} else if (cur_user->facing == WEST) {
		cur_wall->col = cur_user->col - forward;
		cur_wall->row = cur_user->row - side;
	}
}

/* Key for walls in front of the player (pl) that need to be rendered. All
 * numbers are walls that need to be rendered, either front walls (e.g.
 * 2-4, 9-13, 20-24) or side walls (0-1, 5-8, 14-19).
 *
 *	XX 20 XX 21 XX 22 XX 23 XX 24 XX
 *	14 .. 15 .. 16 .. 17 .. 18 .. 19
 *	XX  9 XX 10 XX 11 XX 12 XX 13 XX
 *	XX ..  5 ..  6 ..  7 ..  8 .. XX
 *	XX XX XX  2 XX  3 XX  4 XX XX XX
 *	XX .. XX ..  0 pl  1 .. XX .. XX
 */
 
 /* Key for junk (8 = pl):
  *
  *     XX XX XX XX XX XX XX XX XX XX XX
  *     XX  0 XX  1 XX  2 XX  3 XX  4 XX
  *     XX XX XX XX XX XX XX XX XX XX XX
  *     XX .. XX  5 XX  6 XX  7 XX .. XX
  *     XX XX XX XX XX XX XX XX XX XX XX
  *     XX .. XX .. XX  8 XX .. XX .. XX
  */

static void
set_walls(struct user *cur_user, struct coords walls[25])
{
	/* Set all coordinates based on key above */
	get_coords(cur_user, &walls[0], 0, -1);
	get_coords(cur_user, &walls[1], 0, 1);
	get_coords(cur_user, &walls[2], 1, -2);
	get_coords(cur_user, &walls[3], 1, 0);
	get_coords(cur_user, &walls[4], 1, 2);
	get_coords(cur_user, &walls[5], 2, -3);
	get_coords(cur_user, &walls[6], 2, -1);
	get_coords(cur_user, &walls[7], 2, 1);
	get_coords(cur_user, &walls[8], 2, 3);
	get_coords(cur_user, &walls[9], 3, -4);
	get_coords(cur_user, &walls[10], 3, -2);
	get_coords(cur_user, &walls[11], 3, 0);
	get_coords(cur_user, &walls[12], 3, 2);
	get_coords(cur_user, &walls[13], 3, 4);
	get_coords(cur_user, &walls[14], 4, -5);
	get_coords(cur_user, &walls[15], 4, -3);
	get_coords(cur_user, &walls[16], 4, -1);
	get_coords(cur_user, &walls[17], 4, 1);
	get_coords(cur_user, &walls[18], 4, 3);
	get_coords(cur_user, &walls[19], 4, 5);
	get_coords(cur_user, &walls[20], 5, -4);
	get_coords(cur_user, &walls[21], 5, -2);
	get_coords(cur_user, &walls[22], 5, 0);
	get_coords(cur_user, &walls[23], 5, 2);
	get_coords(cur_user, &walls[24], 5, 4);
}

static void
set_junk(struct user *cur_user, struct coords junk[9])
{
	get_coords(cur_user, &junk[0], 4, -4);
	get_coords(cur_user, &junk[1], 4, -2);
	get_coords(cur_user, &junk[2], 4, 0);
	get_coords(cur_user, &junk[3], 4, 2);
	get_coords(cur_user, &junk[4], 4, 4);
	get_coords(cur_user, &junk[5], 2, -2);
	get_coords(cur_user, &junk[6], 2, 0);
	get_coords(cur_user, &junk[7], 2, 2);
	get_coords(cur_user, &junk[8], 0, 0);
}

static void
check_walls(struct game *cur_game, struct user *cur_user, struct coords walls[25])
{
	int i;
	for (i = 0; i < 25; i++) {
		if (walls[i].row < 0 || walls[i].row > cur_game->maps[cur_user->map].rows - 1 ||
		    walls[i].col < 0 || walls[i].col > cur_game->maps[cur_user->map].cols - 1) {
		    	/* Wall outside of range, set to user position (ROOM) */
		    	walls[i].row = cur_user->row;
		    	walls[i].col = cur_user->col;
		}
	}
}

struct wall_dim {
	int wall_num;
	int sprite;
	SDL_bool flip;
	int overlay;
	SDL_Rect dim;
} wall_dim[25] = {
	{ 0, 1, SDL_FALSE, 0, { -37, -51, 250, 800 } },
	{ 1, 1, SDL_TRUE, 0, { 713, -51, 250, 800 } }, 
	{ 2, 0, SDL_FALSE, 32, { -287, 149, 500, 400 } }, 
	{ 3, 0, SDL_FALSE, 0, { 213, 149, 500, 400 } }, 
	{ 4, 0, SDL_FALSE, 32, { 713, 149, 500, 400 } }, 
	{ 5, 1, SDL_FALSE, 96, { -288, 149, 376, 400 } }, 
	{ 6, 1, SDL_FALSE, 64, { 213, 149, 125, 400 } }, 
	{ 7, 1, SDL_TRUE, 64, { 588, 149, 125, 400 } }, 
	{ 8, 1, SDL_TRUE, 96, { 838, 149, 376, 400 } }, 
	{ 9, 0, SDL_FALSE, 160, { -162, 249, 250, 200 } }, 
	{ 10, 0, SDL_FALSE, 128, { 88, 249, 250, 200 } }, 
	{ 11, 0, SDL_FALSE, 96, { 338, 249, 250, 200 } }, 
	{ 12, 0, SDL_FALSE, 128, { 588, 249, 250, 200 } }, 
	{ 13, 0, SDL_FALSE, 160, { 838, 249, 250, 200 } }, 
	{ 14, 1, SDL_FALSE, 192, { -159, 249, 310, 200 } }, 
	{ 15, 1, SDL_FALSE, 160, { 88, 249, 188, 200 } }, 
	{ 16, 1, SDL_FALSE, 128, { 338, 249, 63, 200 } }, 
	{ 17, 1, SDL_TRUE, 128, { 526, 249, 62, 200 } }, 
	{ 18, 1, SDL_TRUE, 160, { 651, 249, 188, 200 } }, 
	{ 19, 1, SDL_TRUE, 192, { 776, 249, 310, 200 } }, 
	{ 20, 0, SDL_FALSE, 192, { 151, 299, 125, 100 } }, 
	{ 21, 0, SDL_FALSE, 160, { 276, 299, 125, 100 } }, 
	{ 22, 0, SDL_FALSE, 128, { 401, 299, 125, 100 } }, 
	{ 23, 0, SDL_FALSE, 160, { 526, 299, 125, 100 } }, 
	{ 24, 0, SDL_FALSE, 192, { 651, 299, 125, 100 } }
};

struct junk_dim {
	int junk_num;
	int sprite;
	int sprite_add;
	int overlay;
	SDL_Rect dim;
} junk_dim[9] = {
	{ 0, 0, 2, 224, { 86, 367, 64, 64 } },
	{ 1, 0, 2, 208, { 275, 367, 64, 64 } },
	{ 2, 0, 0, 192, { 432, 367, 64, 64 } },
	{ 3, 0, 1, 208, { 589, 367, 64, 64 } },
	{ 4, 0, 1, 224, { 778, 367, 64, 64 } },
	{ 5, 0, 2, 144, { 117, 373, 128, 128 } }, 
	{ 6, 0, 0, 128, { 399, 373, 128, 128 } }, 
	{ 7, 0, 1, 144, { 681, 373, 128, 128 } },
	{ 8, 0, 0, 80, { 335, 421, 256, 256 } }
};

static void
check_junk(struct game *cur_game, struct user *cur_user, struct coords junk[9])
{
	int i;
	int facing;
	for (i = 0; i < 9; i++) {
		if (junk[i].row < 0 || junk[i].row > cur_game->maps[cur_user->map].rows - 1 ||
		    junk[i].col < 0 || junk[i].col > cur_game->maps[cur_user->map].cols - 1) {
		    	/* Junk outside of range */
			junk_dim[i].sprite = 0;
		} else if (*(*(cur_game->maps[cur_user->map].junk + junk[i].row) + junk[i].col) != 0) {
			facing = *(*(cur_game->maps[cur_user->map].junk_face + junk[i].row) + junk[i].col);
			if (facing == cur_user->facing) {
				junk_dim[i].sprite = *(*(cur_game->maps[cur_user->map].junk + junk[i].row) + junk[i].col) + 3;
			} else if (facing == cur_user->facing + 1 || facing == cur_user->facing - 3) {
				junk_dim[i].sprite = *(*(cur_game->maps[cur_user->map].junk + junk[i].row) + junk[i].col) + 9;
			} else if (facing == cur_user->facing + 2 || facing == cur_user->facing - 2) {
				junk_dim[i].sprite = *(*(cur_game->maps[cur_user->map].junk + junk[i].row) + junk[i].col) + 0;
			} else if (facing == cur_user->facing + 3 || facing == cur_user->facing - 1) {
				junk_dim[i].sprite = *(*(cur_game->maps[cur_user->map].junk + junk[i].row) + junk[i].col) + 6;
			}
		} else {
			junk_dim[i].sprite = 0;
		}
	}
}

static void
draw_wall(struct game *cur_game, struct user *cur_user, SDL_Texture **walls, struct coords *wall_coords, int wall_num, int sprite_num)
{
	int x_val = 342;
	int y_val = 11;
	/* Adjust sprite */
	sprite_num *= 8;
	/* Check if you need to render the wall */
	switch(*(*(cur_game->maps[cur_user->map].tiles + wall_coords[wall_num].row) + wall_coords[wall_num].col)) {
		case WALL:
			draw_sprites(cur_game, walls,
				     sprite_num + wall_dim[wall_num].sprite + 4,
				     wall_dim[wall_num].dim.x + x_val,
				     wall_dim[wall_num].dim.y + y_val,
				     wall_dim[wall_num].dim.w,
				     wall_dim[wall_num].dim.h,
				     255,
				     wall_dim[wall_num].flip);
			draw_sprites(cur_game, walls,
				     sprite_num + wall_dim[wall_num].sprite,
				     wall_dim[wall_num].dim.x + x_val,
				     wall_dim[wall_num].dim.y + y_val,
				     wall_dim[wall_num].dim.w,
				     wall_dim[wall_num].dim.h,
				     wall_dim[wall_num].overlay,
				     wall_dim[wall_num].flip);
			break;
		case DOOR:
			draw_sprites(cur_game, walls,
				     sprite_num + wall_dim[wall_num].sprite + 6,
				     wall_dim[wall_num].dim.x + x_val,
				     wall_dim[wall_num].dim.y + y_val,
				     wall_dim[wall_num].dim.w,
				     wall_dim[wall_num].dim.h,
				     255,
				     wall_dim[wall_num].flip);
			draw_sprites(cur_game, walls,
				     sprite_num + wall_dim[wall_num].sprite + 2,
				     wall_dim[wall_num].dim.x + x_val,
				     wall_dim[wall_num].dim.y + y_val,
				     wall_dim[wall_num].dim.w,
				     wall_dim[wall_num].dim.h,
				     wall_dim[wall_num].overlay,
				     wall_dim[wall_num].flip);
			break;
	}
}

static void
draw_junk(struct game *cur_game, SDL_Texture **junk, int junk_num)
{
	int x_val = 342;
	int y_val = 11;
	
	if (junk_dim[junk_num].sprite != 0) {
		draw_sprites(cur_game, junk, junk_dim[junk_num].sprite - 1 + 12 + junk_dim[junk_num].sprite_add,
		junk_dim[junk_num].dim.x + x_val,
		junk_dim[junk_num].dim.y + y_val,
		junk_dim[junk_num].dim.w,
		junk_dim[junk_num].dim.h,
		255, SDL_FALSE);
		draw_sprites(cur_game, junk, junk_dim[junk_num].sprite - 1 + junk_dim[junk_num].sprite_add,
		junk_dim[junk_num].dim.x + x_val,
		junk_dim[junk_num].dim.y + y_val,
		junk_dim[junk_num].dim.w,
		junk_dim[junk_num].dim.h,
		junk_dim[junk_num].overlay, SDL_FALSE);
	}
}

void
draw_view(struct game *cur_game, struct user *cur_user)
{
	int i;
	struct coords walls[25];
	struct coords junk[9];
	int draw_order[25]     = { 20, 21, 22, 23, 24, 14, 15, 16, 17, 19, 18, 9, 10, 11, 12, 13, 5, 6, 7, 8, 2, 3, 4, 0, 1 };
	int draw_order_junk[25] = { -1, 1, 2, 3, -1, 0, -1, -1, -1, 4, -1, -1, -1, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	/* Output to view texture and clear it */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.view);
	SDL_SetRenderDrawColor(cur_game->display.renderer, 0, 0, 0, 255);
	SDL_RenderClear(cur_game->display.renderer);
	/* Draw background */
	draw_rect(cur_game, 342, 11, 927, 698, SDL_TRUE, "black");
	draw_sprites(cur_game, cur_game->sprites.floors, cur_game->maps[cur_user->map].sprite, 342, 11, 927, 698, 255, SDL_FALSE);
	/* Determine values for all walls and junk depending on user facing */
	set_walls(cur_user, walls);
	set_junk(cur_user, junk);
	/* Make sure the walls and junk are within the map range */
	check_walls(cur_game, cur_user, walls);
	check_junk(cur_game, cur_user, junk);
	/* Draw all walls and junk */
	for (i = 0; i < 25; i++) {
		draw_wall(cur_game, cur_user, cur_game->sprites.walls, walls, draw_order[i], cur_game->maps[cur_user->map].sprite);
		if (draw_order_junk[i] != -1) {
			draw_junk(cur_game, cur_game->sprites.junk, draw_order_junk[i]);
		}
	}
	/* Draw junk on the player square */
	draw_junk(cur_game, cur_game->sprites.junk, 8);
	/* Output white outline */
	draw_rect(cur_game, 341, 10, 929, 700, SDL_FALSE, "white");
	/* Reset the render target */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.output);
}
