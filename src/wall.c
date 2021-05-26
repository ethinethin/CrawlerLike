#include <SDL2/SDL.h>
#include "draw.h"
#include "main.h"
#include "user.h"
#include "wall.h"

/* These functions, especially the second, are pretty terrible, so that's why I made a
 * new file for them. There is almost definitely a better way to do this, but for now,
 * this is how it is. There is a key to the wall numbers at the bottom... */

void
draw_map(struct game *cur_game, struct user *cur_user, int x, int y)
{
	int i, j;
	int x_coord, y_coord;
	int map_num;

	/* Draw map */
	map_num =  cur_user->map;
	draw_rect(cur_game, x + 1, y + 1, 319, 319, SDL_TRUE, "darkblue");
	draw_rect(cur_game, x, y, 321, 321, SDL_FALSE, "white");
	for (i = 1; i < cur_game->maps[map_num].rows - 1; i += 2) {
		for (j = 1; j < cur_game->maps[map_num].cols - 1; j += 2) {
			x_coord = j/2;
			y_coord = i/2;
			/* Only draw the walls if the room has been seen */
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
				if (cur_user->row == i && cur_user->col == j) {
					draw_sprites(cur_game, cur_game->sprites.arrows, cur_user->facing, x + x_coord * 20 + 2, y + y_coord * 20 + 2, 16, 16, 255, SDL_FALSE);
				}
			} else {
				draw_rect(cur_game, x + x_coord * 20 + 9, y + y_coord * 20 + 9, 2, 2, SDL_TRUE, "white");
			}
		}
	}
}

void
draw_view(struct game *cur_game, struct user *cur_user)
{
	int i;
	struct coords walls[25];

	/* Set up viewport texture and output to it */
	cur_game->display.view = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1280, 720);
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.view);
	/* Draw black background */
	draw_rect(cur_game, 342, 11, 927, 698, SDL_TRUE, "black");
	/* Determine values for all walls depending on user facing */
	/* The key to wall numbers are given below the function */
	if (cur_user->facing == NORTH) {
		walls[0].row = cur_user->row; walls[0].col = cur_user->col - 1;
		walls[1].row = cur_user->row; walls[1].col = cur_user->col + 1;
		
		walls[2].row = cur_user->row - 1; walls[2].col = cur_user->col - 2;
		walls[3].row = cur_user->row - 1; walls[3].col = cur_user->col;
 		walls[4].row = cur_user->row - 1; walls[4].col = cur_user->col + 2;
 		
  		walls[5].row = cur_user->row - 2; walls[5].col = cur_user->col - 3;
		walls[6].row = cur_user->row - 2; walls[6].col = cur_user->col - 1;
		walls[7].row = cur_user->row - 2; walls[7].col = cur_user->col + 1;
 		walls[8].row = cur_user->row - 2; walls[8].col = cur_user->col + 3;
 		
 		walls[9].row = cur_user->row - 3; walls[9].col = cur_user->col - 4; 		
 		walls[10].row = cur_user->row - 3; walls[10].col = cur_user->col - 2;
 		walls[11].row = cur_user->row - 3; walls[11].col = cur_user->col;
 		walls[12].row = cur_user->row - 3; walls[12].col = cur_user->col + 2;
 		walls[13].row = cur_user->row - 3; walls[13].col = cur_user->col + 4;
 
 		walls[14].row = cur_user->row - 4; walls[14].col = cur_user->col - 5;
		walls[15].row = cur_user->row - 4; walls[15].col = cur_user->col - 3;
		walls[16].row = cur_user->row - 4; walls[16].col = cur_user->col - 1;
		walls[17].row = cur_user->row - 4; walls[17].col = cur_user->col + 1;
 		walls[18].row = cur_user->row - 4; walls[18].col = cur_user->col + 3;
 		walls[19].row = cur_user->row - 4; walls[19].col = cur_user->col + 5;
 		
 		walls[20].row = cur_user->row - 5; walls[20].col = cur_user->col - 4;
 		walls[21].row = cur_user->row - 5; walls[21].col = cur_user->col - 2;
 		walls[22].row = cur_user->row - 5; walls[22].col = cur_user->col;
 		walls[23].row = cur_user->row - 5; walls[23].col = cur_user->col + 2;
 		walls[24].row = cur_user->row - 5; walls[24].col = cur_user->col + 4;
	} else if (cur_user->facing == EAST) {
		walls[0].row = cur_user->row - 1; walls[0].col = cur_user->col;
		walls[1].row = cur_user->row + 1; walls[1].col = cur_user->col;
		
		walls[2].row = cur_user->row - 2; walls[2].col = cur_user->col + 1;
		walls[3].row = cur_user->row; walls[3].col = cur_user->col + 1;
		walls[4].row = cur_user->row + 2; walls[4].col = cur_user->col + 1;
		
		walls[5].row = cur_user->row - 3; walls[5].col = cur_user->col + 2;
		walls[6].row = cur_user->row - 1; walls[6].col = cur_user->col + 2;
		walls[7].row = cur_user->row + 1; walls[7].col = cur_user->col + 2;
		walls[8].row = cur_user->row + 3; walls[8].col = cur_user->col + 2;
		
		walls[9].row = cur_user->row - 4; walls[9].col = cur_user->col + 3;		
		walls[10].row = cur_user->row - 2; walls[10].col = cur_user->col + 3;
		walls[11].row = cur_user->row; walls[11].col = cur_user->col + 3;
		walls[12].row = cur_user->row + 2; walls[12].col = cur_user->col + 3;
		walls[13].row = cur_user->row + 4; walls[13].col = cur_user->col + 3;

		walls[14].row = cur_user->row - 5; walls[14].col = cur_user->col + 4;
		walls[15].row = cur_user->row - 3; walls[15].col = cur_user->col + 4;
		walls[16].row = cur_user->row - 1; walls[16].col = cur_user->col + 4;
		walls[17].row = cur_user->row + 1; walls[17].col = cur_user->col + 4;
		walls[18].row = cur_user->row + 3; walls[18].col = cur_user->col + 4;
		walls[19].row = cur_user->row + 5; walls[19].col = cur_user->col + 4;
		
		walls[20].row = cur_user->row - 4; walls[20].col = cur_user->col + 5;
		walls[21].row = cur_user->row - 2; walls[21].col = cur_user->col + 5;
		walls[22].row = cur_user->row; walls[22].col = cur_user->col + 5;
		walls[23].row = cur_user->row + 2; walls[23].col = cur_user->col + 5;
		walls[24].row = cur_user->row + 4; walls[24].col = cur_user->col + 5;
	} else if (cur_user->facing == SOUTH) {
		walls[0].row = cur_user->row; walls[0].col = cur_user->col + 1;
		walls[1].row = cur_user->row; walls[1].col = cur_user->col - 1;
		
		walls[2].row = cur_user->row + 1; walls[2].col = cur_user->col + 2;
		walls[3].row = cur_user->row + 1; walls[3].col = cur_user->col;
		walls[4].row = cur_user->row + 1; walls[4].col = cur_user->col - 2;
			
		walls[5].row = cur_user->row + 2; walls[5].col = cur_user->col + 3;
		walls[6].row = cur_user->row + 2; walls[6].col = cur_user->col + 1;
		walls[7].row = cur_user->row + 2; walls[7].col = cur_user->col - 1;
		walls[8].row = cur_user->row + 2; walls[8].col = cur_user->col - 3;
		
		walls[9].row = cur_user->row + 3; walls[9].col = cur_user->col + 4;
		walls[10].row = cur_user->row + 3; walls[10].col = cur_user->col + 2;
		walls[11].row = cur_user->row + 3; walls[11].col = cur_user->col;
		walls[12].row = cur_user->row + 3; walls[12].col = cur_user->col - 2;
		walls[13].row = cur_user->row + 3; walls[13].col = cur_user->col - 4;

		walls[14].row = cur_user->row + 4; walls[14].col = cur_user->col + 5;
		walls[15].row = cur_user->row + 4; walls[15].col = cur_user->col + 3;
		walls[16].row = cur_user->row + 4; walls[16].col = cur_user->col + 1;
		walls[17].row = cur_user->row + 4; walls[17].col = cur_user->col - 1;
		walls[18].row = cur_user->row + 4; walls[18].col = cur_user->col - 3;
		walls[19].row = cur_user->row + 4; walls[19].col = cur_user->col - 5;
		
		walls[20].row = cur_user->row + 5; walls[20].col = cur_user->col + 4;
		walls[21].row = cur_user->row + 5; walls[21].col = cur_user->col + 2;
		walls[22].row = cur_user->row + 5; walls[22].col = cur_user->col;
		walls[23].row = cur_user->row + 5; walls[23].col = cur_user->col - 2;
		walls[24].row = cur_user->row + 5; walls[24].col = cur_user->col - 4;
	} else if (cur_user->facing == WEST) {
		walls[0].row = cur_user->row + 1; walls[0].col = cur_user->col;
		walls[1].row = cur_user->row - 1; walls[1].col = cur_user->col;
		
		walls[2].row = cur_user->row + 2; walls[2].col = cur_user->col - 1;
		walls[3].row = cur_user->row; walls[3].col = cur_user->col - 1;
		walls[4].row = cur_user->row - 2; walls[4].col = cur_user->col - 1;
		
		walls[5].row = cur_user->row + 3; walls[5].col = cur_user->col - 2;
		walls[6].row = cur_user->row + 1; walls[6].col = cur_user->col - 2;
		walls[7].row = cur_user->row - 1; walls[7].col = cur_user->col - 2;
		walls[8].row = cur_user->row - 3; walls[8].col = cur_user->col - 2;
		
		walls[9].row = cur_user->row + 4; walls[9].col = cur_user->col - 3;
		walls[10].row = cur_user->row + 2; walls[10].col = cur_user->col - 3;
		walls[11].row = cur_user->row; walls[11].col = cur_user->col - 3;
		walls[12].row = cur_user->row - 2; walls[12].col = cur_user->col - 3;
		walls[13].row = cur_user->row - 4; walls[13].col = cur_user->col - 3;

		walls[14].row = cur_user->row + 5; walls[14].col = cur_user->col - 4;
		walls[15].row = cur_user->row + 3; walls[15].col = cur_user->col - 4;
		walls[16].row = cur_user->row + 1; walls[16].col = cur_user->col - 4;
		walls[17].row = cur_user->row - 1; walls[17].col = cur_user->col - 4;
		walls[18].row = cur_user->row - 3; walls[18].col = cur_user->col - 4;
		walls[19].row = cur_user->row - 5; walls[19].col = cur_user->col - 4;
		
		walls[20].row = cur_user->row + 4; walls[20].col = cur_user->col - 5;
		walls[21].row = cur_user->row + 2; walls[21].col = cur_user->col - 5;
		walls[22].row = cur_user->row; walls[22].col = cur_user->col - 5;
		walls[23].row = cur_user->row - 2; walls[23].col = cur_user->col - 5;
		walls[24].row = cur_user->row - 4; walls[24].col = cur_user->col - 5;
	}
	/* Make sure the wall checks are within the map range - if not, set to user square (no wall) */
	for (i = 0; i < 25; i++) {
		if (walls[i].row < 0 || walls[i].row > cur_game->maps[cur_user->map].rows - 1 ||
		    walls[i].col < 0 || walls[i].col > cur_game->maps[cur_user->map].cols - 1) {
		    	walls[i].row = cur_user->row;
		    	walls[i].col = cur_user->col;
		}
	}
	// reference: window 927 x 698 at (342, 11)
	int x_val = 342; int y_val = 11;
	/* KLMNO - 20, 21, 22, 23, 24 (125x100) */
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
	/* EFGHIJ - 14, 15, 16, 17, 18, 19 (200x62) */
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

// Key: walls in front of the player (p) that need to render
// X K X L X M X N X O X	. = potential room, no rendering needed
// E . F . G . H . I . J	X = part of the grid
// X 9 X A X B X C X D X	1-9, A-O = wall_1, wall_2, ... wall_A, ..etc. above
// X . 5 . 6 . 7 . 8 . X
// X X X 2 X 3 X 4 X X X
// X . X . 0 p 1 . X . X

