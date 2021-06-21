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
static void	make_dungeon(struct map *cur_map);
static void	add_junk(struct map *cur_map);

void
init_map(struct map *cur_map, int rows, int cols)
{
	int i, j;
	
	/* Set up map dimensions */
	cur_map->rows = rows;
	cur_map->cols = cols;
	/* Allocate memory and set to default grid values */
	cur_map->tiles = malloc(sizeof(*cur_map->tiles) * rows);
	cur_map->junk = malloc(sizeof(*cur_map->junk) * rows);
	cur_map->junk_face = malloc(sizeof(*cur_map->junk_face) * rows);
	for (i = 0; i < rows; i++) {
		*(cur_map->tiles + i) = malloc(sizeof(**cur_map->tiles) * cols);
		*(cur_map->junk + i) = malloc(sizeof(*cur_map->junk) * cols);
		*(cur_map->junk_face + i) = malloc(sizeof(*cur_map->junk_face) * rows);
		for (j = 0; j < cols; j++) {
			if (i % 2 == 1 && j % 2 == 1) {
				*(*(cur_map->tiles + i) + j) = UNKNOWN;
			} else {
				*(*(cur_map->tiles + i) + j) = WALL;
			}
			*(*(cur_map->junk + i) + j) = 0;
			*(*(cur_map->junk_face + i) + j) = 0;
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
		free(*(cur_map->junk + i));
		free(*(cur_map->junk_face + i));
	}
	free(cur_map->tiles);
	free(cur_map->junk);
	free(cur_map->junk_face);
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
	/* Remove some random walls, add doors, and add junk */
	make_dungeon(cur_map);
	add_junk(cur_map);
	free(room);
	/* Pick a random wall sprite */
	cur_map->sprite = rand_num(0, 2);
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
make_dungeon(struct map *cur_map)
{
	int i, j;

	for (i = 1; i < cur_map->rows - 1; i++) {
		for (j = 1; j < cur_map->cols - 1; j++) {
			/* Only give a chance to remove the wall if it separates two ROOMS */
			if (*(*(cur_map->tiles + i) + j) == WALL &&
			    ((*(*(cur_map->tiles + i - 1) + j) == ROOM && *(*(cur_map->tiles + i + 1) + j) == ROOM) ||
			     (*(*(cur_map->tiles + i) + j - 1) == ROOM && *(*(cur_map->tiles + i) + j + 1) == ROOM))) {
			    	if (rand_num(0, 99) >= 70) {
			    		*(*(cur_map->tiles + i) + j) = ROOM;
			    		continue;
			    	}
			    	if (rand_num(0, 99) >= 90) {
			    		*(*(cur_map->tiles + i) + j) = DOOR;
			    	}
			}
		}
	}
}

static void
add_junk(struct map *cur_map)
{	
	int i, j;
	int facing;
	int row_d[4] = { -1, 0, 1, 0 };
	int col_d[4] = { 0, 1, 0, -1 };
	
	for (i = 1; i < cur_map->rows; i += 2) {
		for (j = 1; j < cur_map->cols; j += 2) {
			if (*(*(cur_map->tiles + i) + j) == ROOM && rand_num(0, 99) >= 94) {
				*(*(cur_map->junk + i) + j) = 1;
				facing = rand_num(NORTH, WEST);
				*(*(cur_map->junk_face + i) + j) = facing;
				while (*(*(cur_map->tiles + i + row_d[facing]) + j + col_d[facing]) == WALL) {
					facing = rand_num(NORTH, WEST);
					*(*(cur_map->junk_face + i) + j) = facing;
				}
			}
		}
	}	
}
