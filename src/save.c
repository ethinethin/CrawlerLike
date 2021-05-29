#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "home.h"
#include "main.h"
#include "maze.h"
#include "save.h"
#include "user.h"

/* Function prototypes */
static void	save_info(struct game *cur_game, struct user *cur_user, int saveslot);
static void	save_map(struct game *cur_game, int saveslot);
static void	load_map(struct game *cur_game, int saveslot);
static void	save_player(struct user *cur_user, int saveslot);
static void	load_player(struct user *cur_user, int saveslot);

void
save_opts(struct game *cur_game)
{
	FILE *fp;
	
	fp = fopen("options.ini", "w");
	if (fp == NULL) {
		printf("Could not open options.ini\n");
		exit(1);
	}
	
	/* Save the settings from the options screen */
	fprintf(fp, "res=%dx%d\n", cur_game->display.w, cur_game->display.h);
	fprintf(fp, "vsync=%d\n", cur_game->display.vsync);
	fprintf(fp, "displaymode=%d\n", cur_game->display.mode);
	fclose(fp);
}

void
load_opts(struct game *cur_game)
{
	FILE *fp;
	
	fp = fopen("options.ini", "r");
	if (fp == NULL) {
		/* File does not exist, keep the default values */
		return;
	}
	
	/* Load the settings */
	fscanf(fp, "res=%dx%d\n", &cur_game->display.w, &cur_game->display.h);
	cur_game->display.scale_w = cur_game->display.w/1280.0;
	cur_game->display.scale_h = cur_game->display.h/720.0;
	fscanf(fp, "vsync=%d\n", &cur_game->display.vsync);
	fscanf(fp, "displaymode=%d\n", &cur_game->display.mode);
	fclose(fp);
}

SDL_bool checked_directories = SDL_FALSE;
static void
check_directories(void)
{
	char *dirs[] = { "save", "save/save1", "save/save2", "save/save3" };
	int i;
	DIR *dp;

	/* Check if save directories exist */
	for (i = 0; i < 4; i++) {
		dp = opendir(dirs[i]);
		if (dp != NULL) {
			closedir(dp);
		} else {
			mkdir(dirs[i], 0744);
		}
	}
	/* Only need to check once */
	checked_directories = SDL_TRUE;
}

void
get_savefile_info(struct savefile_info *save, int saveslot)
{
	char filename[20];
	FILE *fp;

	/* First time this is accessed, make sure the directories exist */
	if (checked_directories == SDL_FALSE) check_directories();
	/* Open savefile info file and pull out information */
	sprintf(filename, "save/save%d/info.txt", saveslot);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		save->exists = SDL_FALSE;
	} else {
		save->exists = SDL_TRUE;
		fscanf(fp, "%[^\n]%*c", save->line1);
		fscanf(fp, "%[^\n]%*c", save->line2);
		fscanf(fp, "%[^\n]%*c", save->line3);
		fclose(fp);
	}	
}

void
save_all(struct game *cur_game, struct user *cur_user, int saveslot)
{
	cur_game->save = saveslot;
	save_info(cur_game, cur_user, saveslot);
	save_map(cur_game, saveslot);
	save_player(cur_user, saveslot);
}

void
load_all(struct game *cur_game, struct user *cur_user, int saveslot)
{
	load_map(cur_game, saveslot);
	load_player(cur_user, saveslot);
	cur_game->state = LOADED;
	cur_game->save = saveslot;
}

static void
save_info(struct game *cur_game, struct user *cur_user, int saveslot)
{
	char filename[20];
	FILE *fp;
	time_t timestamp;
 
	time(&timestamp);
	
	sprintf(filename, "save/save%d/info.txt", saveslot);
	fp = fopen(filename, "w");
	/* This stuff needs to come from the character */
	fprintf(fp, "%s\n%s\n%s\n", "Mr. Bobby (Lv. 20)", "Floor: 37", ctime(&timestamp));
	fclose(fp);
}

static void
save_map(struct game *cur_game, int saveslot)
{
	char filename[20];
	int map, i, j;
	FILE *fp;
	
	sprintf(filename, "save/save%d/maps.txt", saveslot);
	fp = fopen(filename, "w");
	/* Output dimensions */
	fprintf(fp, "num_maps=%d\n", cur_game->num_maps);
	fprintf(fp, "rows=%d\n", cur_game->maps[0].rows);
	fprintf(fp, "cols=%d\n", cur_game->maps[0].cols);
	/* Output each map */
	for (map = 0; map < cur_game->num_maps; map++) {
		for (i = 0; i < cur_game->maps[map].rows; i++) {
			for (j = 0; j < cur_game->maps[map].cols; j++) {
				fprintf(fp, "%d", *(*(cur_game->maps[map].tiles + i) + j));
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

static void
load_map(struct game *cur_game, int saveslot)
{
	char filename[20];
	int map, i, j;
	int rows, cols;
	FILE *fp;
	
	sprintf(filename, "save/save%d/maps.txt", saveslot);
	fp = fopen(filename, "r");
	/* Read in dimensions */
	fscanf(fp, "num_maps=%d\n", &cur_game->num_maps);
	fscanf(fp, "rows=%d\n", &rows);
	fscanf(fp, "cols=%d\n", &cols);
	/* Allocate memory */
	cur_game->maps = malloc(sizeof(*cur_game->maps) * cur_game->num_maps);
	/* Input each map */
	for (map = 0; map < cur_game->num_maps; map++) {
		init_map(&cur_game->maps[map], rows, cols);
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				*(*(cur_game->maps[map].tiles + i) + j) = fgetc(fp) - 48;
			}
		}
		fscanf(fp, "\n", NULL);
	}
	fclose(fp);
}

static void
save_player(struct user *cur_user, int saveslot)
{
	char filename[20];
	int map, i, j;
	FILE *fp;
	
	sprintf(filename, "save/save%d/user.txt", saveslot);
	fp = fopen(filename, "w");
	/* Output current location */
	fprintf(fp, "map=%d\n", cur_user->map);
	fprintf(fp, "row=%d\n", cur_user->row);
	fprintf(fp, "col=%d\n", cur_user->col);
	fprintf(fp, "facing=%d\n", cur_user->facing);
	/* Output seen dimensions */
	fprintf(fp, "num_seen=%d\n", cur_user->num_seen);
	fprintf(fp, "seen_rows=%d\n", cur_user->seen[0].rows);
	fprintf(fp, "seen_cols=%d\n", cur_user->seen[0].cols);
	/* Output all seen data */
	for (map = 0; map < cur_user->num_seen; map++) {
		for (i = 0; i < cur_user->seen[0].rows; i++) {
			for (j = 0; j < cur_user->seen[0].cols; j++) {
				fprintf(fp, "%d", *(*(cur_user->seen[map].tiles + i) + j));
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

static void
load_player(struct user *cur_user, int saveslot)
{
	char filename[20];
	int map, i, j;
	int rows, cols;
	FILE *fp;
	
	sprintf(filename, "save/save%d/user.txt", saveslot);
	fp = fopen(filename, "r");
	/* Input player location */
	fscanf(fp, "map=%d\n", &cur_user->map);
	fscanf(fp, "row=%d\n", &cur_user->row);
	fscanf(fp, "col=%d\n", &cur_user->col);
	fscanf(fp, "facing=%d\n", &cur_user->facing);
	/* Input seen dimensions */
	fscanf(fp, "num_seen=%d\n", &cur_user->num_seen);
	fscanf(fp, "seen_rows=%d\n", &rows);
	fscanf(fp, "seen_cols=%d\n", &cols);
	/* Allocate memory */
	cur_user->seen = malloc(sizeof(*cur_user->seen) * cur_user->num_seen);
	/* Input all seen data */
	for (map = 0; map < cur_user->num_seen; map++) {
		init_seen(&cur_user->seen[map], rows, cols);
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				*(*(cur_user->seen[map].tiles + i) + j) = fgetc(fp) - 48;
			}
		}
		fscanf(fp, "\n", NULL);
	}
	fclose(fp);
}
