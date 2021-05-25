#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "draw.h"
#include "main.h"
#include "maze.h"
#include "rand.h"

/* Function prototypes */
static void	expand_rooms(struct map *cur_map, int row, int col);
static int	count_potential_rooms(struct map *cur_map);
static void	rand_potential_room(struct map *cur_map, struct coords *pos, int count);
static int	maze_entrances(struct map *cur_map, int row, int col);
static void	remove_wall(struct map *cur_map, int row, int col, int count);
static int	count_rooms(struct map *cur_map);
static void	remove_random_walls(struct map *cur_map);


void
init_map(struct map *cur_map, int rows, int cols)
{
	int i, j;
	
	/* Set up map dimensions */
	cur_map->rows = rows;
	cur_map->cols = cols;
	/* Allocate memory and set to default grid values */
	cur_map->tiles = malloc(sizeof(*cur_map->tiles) * rows);
	for (i = 0; i < rows; i++) {
		*(cur_map->tiles + i) = malloc(sizeof(**cur_map->tiles) * cols);
		for (j = 0; j < cols; j++) {
			if (i % 2 == 1 && j % 2 == 1) {
				*(*(cur_map->tiles + i) + j) = UNKNOWN;
			} else {
				*(*(cur_map->tiles + i) + j) = WALL;
			}
		}
	}		
}

void
kill_map(struct map *cur_map)
{
	int i;

	/* Free memory */
	for (i = 0; i < cur_map->rows; i++) {
		free(*(cur_map->tiles + i));
	}
	free(cur_map->tiles);
}

void
populate_map(struct map *cur_map, int row_start, int col_start)
{
	int count;
	struct coords *room;
	
	/* Make sure the starting position is an UNKNOWN room or exit */
	if (*(*(cur_map->tiles + row_start) + col_start) != UNKNOWN) {
		printf("Starting coordinates (row: %d, col: %d) should be odd.\n", row_start, col_start);
		exit(1);
	}
	/* Make the given room a room and expand potential rooms around it */
	*(*(cur_map->tiles + row_start) + col_start) = ROOM;
	expand_rooms(cur_map, row_start, col_start);
	/* Loop as long as there are potential rooms */
	room = malloc(sizeof(*room));
	while ((count = count_potential_rooms(cur_map)) > 0) {
		/* Pick a random potential room */
		rand_potential_room(cur_map, room, count);
		/* Remove one of the walls around it */
		remove_wall(cur_map, room->row, room->col, maze_entrances(cur_map, room->row, room->col));
		/* Expand potential rooms around it */
		expand_rooms(cur_map, room->row, room->col);
		/* Random chance to exit loop */
		if (count_rooms(cur_map) >= 81 && rand_num(0, 99) >= 90) break;
	}
	/* Set up start and end coordinates */
	*(*(cur_map->tiles + row_start) + col_start) = START;
	cur_map->start.row = row_start;
	cur_map->start.col = col_start;
	*(*(cur_map->tiles + room->row) + room->col) = END;
	cur_map->end.row = room->row;
	cur_map->end.col = room->col;
	/* Remove some random walls */
	remove_random_walls(cur_map);
	free(room);
}

static void
expand_rooms(struct map *cur_map, int row, int col)
{
	/* Make all UNKNOWN rooms around given room POTENTIAL rooms */
	if (row > 2 && *(*(cur_map->tiles + row - 2) + col) == UNKNOWN) {
		*(*(cur_map->tiles + row - 2) + col) = POTENTIAL;
	}
	if (col < cur_map->cols - 2 && *(*(cur_map->tiles + row) + col + 2) == UNKNOWN)  {
		*(*(cur_map->tiles + row) + col + 2) = POTENTIAL;
	}
	if (row < cur_map->rows - 2 && *(*(cur_map->tiles + row + 2) + col) == UNKNOWN) {
		*(*(cur_map->tiles + row + 2) + col) = POTENTIAL;
	}
	if (col > 2 && *(*(cur_map->tiles + row) + col - 2) == UNKNOWN) {
		*(*(cur_map->tiles + row) + col - 2) = POTENTIAL;
	}
}

static int
count_potential_rooms(struct map *cur_map)
{
	int i, j;
	int count;
	
	for (i = 0, count = 0; i < cur_map->rows - 1; i++) {
		for (j = 0; j < cur_map->cols - 1; j++) {
			if (*(*(cur_map->tiles + i) + j) == POTENTIAL) count++;
		}
	}
	return count;
}

static void
rand_potential_room(struct map *cur_map, struct coords *pos, int count)
{
	int i, j, room, room_count;
	
	/* Generate a random number based on count */
	room = rand_num(0, count - 1);
	room_count = 0;
	/* Cycle through all POTENTIAL rooms until you hit the random number */
	 for (i = 0; i < cur_map->rows; i++) {
	 	for (j = 0; j < cur_map->cols; j++) {
	 		if (*(*(cur_map->tiles + i) + j) == POTENTIAL) {
	 			if (room_count == room) {
	 				pos->row = i;
	 		    		pos->col = j;
	 		    		return;
	 		    	} else {
		 			room_count++;
		 		}
	 		}
	 	}
	 }
}

static int
maze_entrances(struct map *cur_map, int row, int col)
{
	int count;
	
	/* Count number of ways into the open maze from this space */
	count = 0;
	if (row > 2 && *(*(cur_map->tiles + row - 2) + col) == ROOM) count++;
	if (col < cur_map->cols - 2 && *(*(cur_map->tiles + row) + col + 2) == ROOM) count++;
	if (row < cur_map->rows - 2 && *(*(cur_map->tiles + row + 2) + col) == ROOM) count++;
	if (col > 2 && *(*(cur_map->tiles + row) + col - 2) == ROOM) count++;
	return count;
}

static void
remove_wall(struct map *cur_map, int row, int col, int count)
{
	/* Remove a wall to open this room to the open maze */
	if (row > 2 && *(*(cur_map->tiles + row - 2) + col) == ROOM &&
	    (count == 1 || rand_num(0, 99) == 99)) {
	    	*(*(cur_map->tiles + row) + col) = ROOM;
	    	*(*(cur_map->tiles + row - 1) + col) = ROOM;
	    	return;
	} else if (col < cur_map->cols - 2 && *(*(cur_map->tiles + row) + col + 2) == ROOM &&
	    (count == 1 || rand_num(0, 99) == 99)) {
	    	*(*(cur_map->tiles + row) + col) = ROOM;
	    	*(*(cur_map->tiles + row) + col + 1) = ROOM;
	    	return;
	} else if (row < cur_map->rows - 2 && *(*(cur_map->tiles + row + 2) + col) == ROOM &&
	    (count == 1 || rand_num(0, 99) == 99)) {
	    	*(*(cur_map->tiles + row) + col) = ROOM;
	    	*(*(cur_map->tiles + row + 1) + col) = ROOM;
	    	return;
	} else if (col > 2 && *(*(cur_map->tiles + row) + col - 2) == ROOM &&
	    (count == 1 || rand_num(0, 99) == 99)) {
	    	*(*(cur_map->tiles + row) + col) = ROOM;
	    	*(*(cur_map->tiles + row) + col - 1) = ROOM;
	    	return;
	}
	/* Made it to the end without a jackpot winner, try again */
	remove_wall(cur_map, row, col, count);
}

static int
count_rooms(struct map *cur_map)
{
	int i, j, count;
	
	for (i = 1, count = 0; i < cur_map->rows; i += 2) {
		for (j = 1; j < cur_map->cols; j += 2) {
			if (*(*(cur_map->tiles + i) + j) == ROOM) count++;
		}
	}
	return count;
}

static void
remove_random_walls(struct map *cur_map)
{
	int i, j;

	for (i = 1; i < cur_map->rows - 1; i++) {
		for (j = 1; j < cur_map->cols - 1; j++) {
			/* Only give a chance to remove the wall if it separates two ROOMS */
			if (*(*(cur_map->tiles + i) + j) == WALL &&
			    ((*(*(cur_map->tiles + i - 1) + j) == ROOM && *(*(cur_map->tiles + i + 1) + j) == ROOM) ||
			     (*(*(cur_map->tiles + i) + j - 1) == ROOM && *(*(cur_map->tiles + i) + j + 1) == ROOM))) {
			    	if (rand_num(0, 99) >= 80) {
			    		*(*(cur_map->tiles + i) + j) = ROOM;
			    	}
			}
		}
	}
}

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
draw_view(struct game *cur_game, struct user *cur_user) /* This is a pretty gross function, but... I guess it works */
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
  		walls[5].row = cur_user->row - 2; walls[6].col = cur_user->col - 3;
		walls[6].row = cur_user->row - 2; walls[6].col = cur_user->col - 1;
		walls[7].row = cur_user->row - 2; walls[7].col = cur_user->col + 1;
 		walls[8].row = cur_user->row - 2; walls[8].col = cur_user->col + 3;
 		walls[9].row = cur_user->row - 3; walls[9].col = cur_user->col - 4; 		
 		walls[10].row = cur_user->row - 3; walls[10].col = cur_user->col - 2;
 		walls[11].row = cur_user->row - 3; walls[11].col = cur_user->col;
 		walls[12].row = cur_user->row - 3; walls[12].col = cur_user->col + 2;
 		walls[13].row = cur_user->row - 3; walls[13].col = cur_user->col + 4;
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
	}
	/* Make sure the wall checks are within the map range */
	for (i = 0; i < 23; i++) {
		if (walls[i].row < 0 || walls[i].row > cur_game->maps[cur_user->map].rows - 1 ||
		    walls[i].col < 0 || walls[i].col > cur_game->maps[cur_user->map].cols - 1) {
		    	walls[i].row = cur_user->row;
		    	walls[i].col = cur_user->col;
		}
	}
	// reference: window 927 x 698 at (342, 11)
	int x_val = 342; int y_val = 11;
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
	}	/* If there are walls to the side in front of you, draw them (400x125) */
	if (*(*(cur_game->maps[cur_user->map].tiles + walls[5].row) + walls[5].col) == WALL) {
		draw_rect(cur_game, -37 + x_val, 149 + y_val, 125, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, -37 + x_val, 149 + y_val, 125, 400, 205, SDL_FALSE);
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
		draw_rect(cur_game, 838 + x_val, 149 + y_val, 125, 400, SDL_TRUE, "black");
		draw_sprites(cur_game, cur_game->sprites.walls, 1, 838 + x_val, 149 + y_val, 125, 400, 205, SDL_TRUE);
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
// X K X L X M X N X O X
// E . F . G . H . I . J
// X 9 X A X B X C X D X
// X . 5 . 6 . 7 . 8 . X
// X X X 2 X 3 X 4 X X X
// X . X . 0 p 1 . X . X
