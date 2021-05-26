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
static void	check_walls(struct game *cur_game, struct user *cur_user, struct coords walls[25]);

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
				if (cur_game->maps[map_num].tiles[i - 1][j] == WALL) {
					draw_line(cur_game, x + x_coord * 20, y + y_coord * 20, x + (x_coord + 1) * 20, y + y_coord * 20, "white");
				}
				if (cur_game->maps[map_num].tiles[i][j + 1] == WALL) {
					draw_line(cur_game, x + (x_coord + 1) * 20, y + y_coord * 20, x + (x_coord + 1) * 20, y + (y_coord + 1) * 20, "white");
				}
				if (cur_game->maps[map_num].tiles[i + 1][j] == WALL) {
					draw_line(cur_game, x + x_coord * 20, y + (y_coord + 1) * 20, x + (x_coord + 1) * 20, y + (y_coord + 1) * 20, "white");
				}
				if (cur_game->maps[map_num].tiles[i][j - 1] == WALL) {
					draw_line(cur_game, x + x_coord * 20, y + y_coord * 20, x + x_coord * 20, y + (y_coord + 1) * 20, "white");
				}
				if (cur_game->maps[map_num].tiles[i][j] == START) {
					draw_rect(cur_game, x + x_coord * 20 + 5, y + y_coord * 20 + 5, 10, 10, SDL_TRUE, "white");
				} else if (cur_game->maps[map_num].tiles[i][j] == END) {
					draw_rect(cur_game, x + x_coord * 20 + 7, y + y_coord * 20 + 7, 6, 6, SDL_TRUE, "white");
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

void
draw_view(struct game *cur_game, struct user *cur_user)
{
	struct coords walls[25];

	/* Set up viewport texture and output to it */
	cur_game->display.view = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1280, 720);
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.view);
	/* Draw black background */
	draw_rect(cur_game, 342, 11, 927, 698, SDL_TRUE, "black");
	/* Determine values for all walls depending on user facing */
	set_walls(cur_user, walls);
	/* Make sure the wall checks are within the map range */
	check_walls(cur_game, cur_user, walls);
	/* View window is 927 x 698 at (342, 11) */
	int x_val = 342; int y_val = 11;
	/* 20, 21, 22, 23, 24 (125x100) */
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[20].row) + walls[20].col) == WALL) {
		draw_rect(cur_game, 151 + x_val, 299 + y_val, 125, 100, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 151 + x_val, 299 + y_val, 125, 100, 105, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[21].row) + walls[21].col) == WALL) {
		draw_rect(cur_game, 276 + x_val, 299 + y_val, 125, 100, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 276 + x_val, 299 + y_val, 125, 100, 105, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[22].row) + walls[22].col) == WALL) {
		draw_rect(cur_game, 401 + x_val, 299 + y_val, 125, 100, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 401 + x_val, 299 + y_val, 125, 100, 105, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[23].row) + walls[23].col) == WALL) {
		draw_rect(cur_game, 526 + x_val, 299 + y_val, 125, 100, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 526 + x_val, 299 + y_val, 125, 100, 105, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[24].row) + walls[24].col) == WALL) {
		draw_rect(cur_game, 651 + x_val, 299 + y_val, 125, 100, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 651 + x_val, 299 + y_val, 125, 100, 105, SDL_FALSE);
	}
	/* 14, 15, 16, 17, 18, 19 (200x62) */
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[14].row) + walls[14].col) == WALL) {
		draw_rect(cur_game, -159 + x_val, 249 + y_val, 310, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, -159 + x_val, 249 + y_val, 310, 200, 105, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[15].row) + walls[15].col) == WALL) {
		draw_rect(cur_game, 88 + x_val, 249 + y_val, 188, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 88 + x_val, 249 + y_val, 188, 200, 130, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[16].row) + walls[16].col) == WALL) {
		draw_rect(cur_game, 339 + x_val, 249 + y_val, 62, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 339 + x_val, 249 + y_val, 62, 200, 155, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[17].row) + walls[17].col) == WALL) {
		draw_rect(cur_game, 526 + x_val, 249 + y_val, 62, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 526 + x_val, 249 + y_val, 62, 200, 155, SDL_TRUE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[19].row) + walls[19].col) == WALL) {
		/* 19 overlaps 18 so draw it first */
		draw_rect(cur_game, 776 + x_val, 249 + y_val, 310, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 776 + x_val, 249 + y_val, 310, 200, 105, SDL_TRUE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[18].row) + walls[18].col) == WALL) {
		draw_rect(cur_game, 651 + x_val, 249 + y_val, 188, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 651 + x_val, 249 + y_val, 188, 200, 130, SDL_TRUE);
	}
	/* If there are walls one space in front of you, draw them (250x200) */
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[9].row) + walls[9].col) == WALL) {
		draw_rect(cur_game, -162 + x_val, 249 + y_val, 250, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, -162 + x_val, 249 + y_val, 250, 200, 180, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[10].row) + walls[10].col) == WALL) {
		draw_rect(cur_game, 88 + x_val, 249 + y_val, 250, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 88 + x_val, 249 + y_val, 250, 200, 180, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[11].row) + walls[11].col) == WALL) {
		draw_rect(cur_game, 338 + x_val, 249 + y_val, 250, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 338 + x_val, 249 + y_val, 250, 200, 180, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[12].row) + walls[12].col) == WALL) {
		draw_rect(cur_game, 588 + x_val, 249 + y_val, 250, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 588 + x_val, 249 + y_val, 250, 200, 180, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[13].row) + walls[13].col) == WALL) {
		draw_rect(cur_game, 838 + x_val, 249 + y_val, 250, 200, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 838 + x_val, 249 + y_val, 250, 200, 180, SDL_FALSE);
	}
	/* If there are walls to the side in front of you, draw them (400x125) */
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[5].row) + walls[5].col) == WALL) {
		draw_rect(cur_game, -288 + x_val, 149 + y_val, 376, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, -288 + x_val, 149 + y_val, 376, 400, 180, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[6].row) + walls[6].col) == WALL) {
		draw_rect(cur_game, 213 + x_val, 149 + y_val, 125, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 213 + x_val, 149 + y_val, 125, 400, 205, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[7].row) + walls[7].col) == WALL) {
		draw_rect(cur_game, 588 + x_val, 149 + y_val, 125, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 588 + x_val, 149 + y_val, 125, 400, 205, SDL_TRUE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[8].row) + walls[8].col) == WALL) {
		draw_rect(cur_game, 838 + x_val, 149 + y_val, 376, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 838 + x_val, 149 + y_val, 376, 400, 180, SDL_TRUE);
	}
	/* If there are walls in front of you, draw them (500x400) */
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[2].row) + walls[2].col) == WALL) {
		draw_rect(cur_game, -287 + x_val, 149 + y_val, 500, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, -287 + x_val, 149 + y_val, 500, 400, 230, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[3].row) + walls[3].col) == WALL) {
		draw_rect(cur_game, 213 + x_val, 149 + y_val, 500, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 213 + x_val, 149 + y_val, 500, 400, 230, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[4].row) + walls[4].col) == WALL) {
		draw_rect(cur_game, 713 + x_val, 149 + y_val, 500, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 0, 713 + x_val, 149 + y_val, 500, 400, 230, SDL_FALSE);
	}
	/* If there are walls to the sides, draw them (800x250) */
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[0].row) + walls[0].col) == WALL) {
		draw_rect(cur_game, -37 + x_val, -51 + y_val, 250, 800, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, -37 + x_val, -51 + y_val, 250, 800, 255, SDL_FALSE);
	}
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[1].row) + walls[1].col) == WALL) {
		draw_rect(cur_game, 713 + x_val, -51 + y_val, 250, 800, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 713 + x_val, -51 + y_val, 250, 800, 255, SDL_TRUE);
	}
	/* Output white grid */
	draw_rect(cur_game, 341, 10, 929, 700, SDL_FALSE, "white");
	/* Reset the render target */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.output);
}
